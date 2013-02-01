#include "debug.h"
#include "math/math.h"
#include "gui/opengl/font.h"

#include <QPainter>
#include <QImage>

namespace MR
{
  namespace GUI
  {
    namespace GL
    {

      namespace {

        const char* vertex_shader_source = 
          "layout(location = 0) in vec2 pos;\n"
          "layout(location = 1) in vec2 font_pos;\n"
          "uniform float scale_x;\n"
          "uniform float scale_y;\n"
          "out vec2 tex_coord;\n"
          "void main () {\n"
          "  gl_Position = vec4 (pos[0]*scale_x-1.0, pos[1]*scale_y-1.0, 0.0, 1.0);\n"
          "  tex_coord = font_pos;\n"
          "}\n";
          
        const char* fragment_shader_source = 
          "in vec2 tex_coord;\n"
          "uniform sampler2D sampler;\n"
          "uniform float red, green, blue;\n"
          "out vec4 color;\n"
          "void main () {\n"
          "  color.ra = texture (sampler, tex_coord).ra;\n"
          "  color.rgb = color.r * vec3 (red, green, blue);\n"
          "}\n";
          
      }





      void Font::initGL () 
      {
        const int first_char = ' ', last_char = '~', default_char = '?';
        INFO ("loading font into OpenGL texture...");

        vertex_buffer_ID[0] = vertex_buffer_ID[1] = 0;

        font_height = metric.height() + 2;
        const float max_font_width = metric.maxWidth() + 2;

        int tex_width = 0;
        for (int c = first_char; c <= last_char; ++c) 
          tex_width += metric.width (c) + 2;

        QImage pixmap (max_font_width, font_height, QImage::Format_ARGB32);
        const GLubyte* pix_data = pixmap.bits();

        float tex_data [2 * tex_width * font_height];

        QPainter painter (&pixmap);
        painter.setFont (font);
        painter.setRenderHints (QPainter::TextAntialiasing);
        painter.setPen (Qt::white);
        
        for (size_t n = 0; n < 256; ++n) 
          font_tex_pos[n] = NAN;

        int current_x = 0;
        for (int c = first_char; c <= last_char; ++c) {
          pixmap.fill (0);
          painter.drawText (1, metric.ascent() + 1, QString(c));

          font_width[c] = metric.width (c);
          const int current_font_width = font_width[c] + 2;

          // blur along x:
          for (int row = 0; row < font_height; ++row) {
            for (int col = 0; col < current_font_width; ++col) {
              const int tex_idx = 2 * (current_x + col + row*tex_width);
              const int pix_idx = 4 * (col + row*max_font_width);
              float val = 0.0f;
              for (int x = -1; x <= 1; ++x)
                if (col+x >= 0 && col+x < current_font_width) 
                  val += Math::exp (-x*x/2.0f) * pix_data[pix_idx+4*x];
              tex_data[tex_idx] = val;
            }
          }

          // blur along y and store as alpha component:
          for (int row = 0; row < font_height; ++row) {
            for (int col = 0; col < current_font_width; ++col) {
              const int tex_idx = 2 * (current_x + col + row*tex_width);
              const int pix_idx = 4 * (col + row*max_font_width);
              float val = 0.0f;
              for (int x = -1; x <= 1; ++x) 
                if (row+x >= 0 && row+x < font_height) 
                  val += Math::exp (-x*x/2.0) * tex_data[tex_idx+2*tex_width*x];
              tex_data[tex_idx+1] = pix_data[pix_idx] ? 1.0f : 0.005f*val;
            }
          }

          // store un-blurred version as luminance component:
          for (int row = 0; row < font_height; ++row) {
            for (int col = 0; col < current_font_width; ++col) {
              const int tex_idx = 2 * (current_x + col + row*tex_width);
              const int pix_idx = 4 * (col + row*max_font_width);
              tex_data[tex_idx] = pix_data[pix_idx] / 255.0f;
            }
          }

          font_tex_pos[c] = current_x;
          font_tex_width[c] = current_font_width;

          current_x += current_font_width;
        }

        for (int n = first_char; n <= last_char; ++n) {
          font_tex_pos[n] /= float (current_x);
          font_tex_width[n] /= float (current_x);
        }

        for (int n = 0; n < 256; ++n) {
          if (!finite (font_tex_pos[n])) {
            font_width[n] = font_width[default_char];
            font_tex_pos[n] = font_tex_pos[default_char];
            font_tex_width[n] = font_tex_width[default_char];
          }
        }

        glGenTextures (1, &tex_ID);
        glBindTexture (GL_TEXTURE_2D, tex_ID);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, tex_width, font_height, 
            0, GL_LUMINANCE_ALPHA, GL_FLOAT, tex_data);

        glGenBuffers (2, vertex_buffer_ID);

        glGenVertexArrays (1, &vertex_array_object_ID);
        glBindVertexArray (vertex_array_object_ID);

        glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_ID[0]);
        glEnableVertexAttribArray (0);
        glVertexAttribPointer (0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_ID[1]);
        glEnableVertexAttribArray (1);
        glVertexAttribPointer (1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        GL::Shader::Vertex vertex_shader (vertex_shader_source);
        GL::Shader::Fragment fragment_shader (fragment_shader_source);

        program.attach (vertex_shader);
        program.attach (fragment_shader);
        program.link();

        INFO ("font loaded");
      }




        Font::~Font () 
        {
          if (tex_ID)
            glDeleteTextures (1, &tex_ID);
          if (vertex_buffer_ID[0])
            glDeleteBuffers (2, vertex_buffer_ID);
          if (vertex_array_object_ID)
            glDeleteVertexArrays (1, &vertex_array_object_ID);
        }




        void Font::render (const std::string& text, int x, int y) const
        {
          assert (tex_ID);
          assert (vertex_buffer_ID[0]);
          assert (vertex_buffer_ID[1]);
          assert (vertex_array_object_ID);

          GLfloat screen_pos [8*text.size()];
          GLfloat tex_pos [8*text.size()];

          x -= 1;
          y -= 1;

          for (size_t n = 0; n < text.size(); ++n) {
            const int c = text[n];
            GLfloat* pos = &screen_pos[8*n];
            pos[0] = x; pos[1] = y;
            pos[2] = x; pos[3] = y + font_height;
            pos[4] = x+font_width[c]+2; pos[5] = y + font_height;
            pos[6] = x+font_width[c]+2; pos[7] = y;

            GLfloat* tex = &tex_pos[8*n];
            tex[0] = font_tex_pos[c]; tex[1] = 1.0;
            tex[2] = font_tex_pos[c]; tex[3] = 0.0;
            tex[4] = font_tex_pos[c]+font_tex_width[c]; tex[5] = 0.0;
            tex[6] = font_tex_pos[c]+font_tex_width[c]; tex[7] = 1.0;

            x += font_width[c];
          }


          glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_ID[0]);
          glBufferData (GL_ARRAY_BUFFER, sizeof (screen_pos), screen_pos, GL_STREAM_DRAW);

          glBindBuffer (GL_ARRAY_BUFFER, vertex_buffer_ID[1]);
          glBufferData (GL_ARRAY_BUFFER, sizeof (tex_pos), tex_pos, GL_STREAM_DRAW);

          glBindTexture (GL_TEXTURE_2D, tex_ID);

          glBindVertexArray (vertex_array_object_ID);
          glDrawArrays (GL_QUADS, 0, 8*text.size());
        }

      }
    }
  }


