#include "common.h"

using namespace std;

void log_impl(ostream& os) {
	os << endl;
}

float random_float() {
	return static_cast<float>(rand()) / (RAND_MAX);
}

static string read_file(const string& filename) {
    ifstream file(filename, ios::binary);

    if (!file) {
        log_t(CON_RED "ERROR! Failed to open file ", filename.c_str());
        return {};
    }

    ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

GLuint compile_shader(const char* vertex_shader_path, const char* fragment_shader_path) {
    int success;
    char info[512];

	log_t("Compiling shaders... ", CON_RED, vertex_shader_path, CON_NORMAL, " / ", CON_RED, fragment_shader_path);

    unsigned int vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    string vertex_shader_source = read_file(vertex_shader_path);
    const char* vertex_source_cstr = vertex_shader_source.c_str();

    glShaderSource(vertex_shader, 1, &vertex_source_cstr, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(vertex_shader, 512, NULL, info);
        log_t(CON_RED "ERROR! Failed to compile vertex shader:\n", info);
    }

    unsigned int fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    string fragment_shader_source = read_file(fragment_shader_path);
    const char* fragment_source_cstr = fragment_shader_source.c_str();

    glShaderSource(fragment_shader, 1, &fragment_source_cstr, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(fragment_shader, 512, NULL, info);
        log_t(CON_RED "ERROR! Failed to compile fragment shader:\n", info);
    }

    unsigned int program = glCreateProgram();

    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(program, 512, NULL, info);
        log_t("Failed to link shaders: \n", info);
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}
