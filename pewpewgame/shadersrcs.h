#pragma once
//																SOLID BOX
const char* solidboxvert =
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";
const char* solidboxfrag =
"#version 460 core\n"
"out vec4 FragColor;\n"
"uniform vec3 color;\n"
"void main()\n"
"{\n"
"  FragColor = vec4(color, 1.0f);\n"
"}\0";
//																TEXT
const char* textvert =
"#version 330 core\n"
"layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>\n"
"out vec2 TexCoords;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);\n"
"    TexCoords = vertex.zw;\n"
"}\0";
const char* textfrag =
"#version 330 core\n"
"in vec2 TexCoords;\n"
"out vec4 color;\n"
"uniform sampler2D text;\n"
"const vec3 textColor=vec3(0.0f,0.0f,0.0f);//uniform vec3 textColor;\n"
"void main()\n"
"{    \n"
"    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);\n"
"    color = vec4(textColor, 1.0) * sampled;\n"
"}\0";
//																GRADIENT
const char* gradientvert =
"#version 460 core\n"
"layout (location = 0) in vec2 aPos;\n"
"void main()\n"
"{\n"
"  gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);\n"
"}\0";
const char* gradientfrag =
"#version 460 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"  FragColor = vec4(gl_FragCoord.x/1280.0f,gl_FragCoord.y/720.0f,0.5f, 1.0f);\n"
"}\0";