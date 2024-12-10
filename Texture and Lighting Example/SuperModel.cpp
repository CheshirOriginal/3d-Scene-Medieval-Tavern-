#include "SuperModel.h"
#include "func.h"
#include "globals.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

SuperModel::SuperModel(const std::string& objPath, const std::string& mtlPath)
{
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = mtlPath; // Путь к файлам материалов

    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(objPath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "TinyObjReader: " << reader.Error();
        }
        exit(1);
    }

    if (!reader.Warning().empty()) {
        std::cout << "TinyObjReader: " << reader.Warning();
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();
    std::map<std::string, GLuint> textureMap; // Хранение текстур по имени материала

    // Загрузка текстур
    auto a = 0;
    for (const auto& mat : materials) {
        if (!mat.diffuse_texname.empty()) {
            //возможно понадобится добавить загрузку разных видов тектур
            //std::cout << mat.diffuse_texname << std::endl;
            GLuint textureID = LoadTexture(mat.diffuse_texname);
            textureMap[mat.name] = textureID;
            a++;
        }
    }
    std::cout << shapes.size() << std::endl;
    std::cout << a << std::endl;

    std::map<GLuint, std::vector<glm::vec3>> vertices_coords;
    std::map<GLuint, std::vector<glm::vec3>> vertices_normals;
    std::map<GLuint, std::vector<glm::vec2>> textures_coords;

    // Разбор каждой формы (shape) в OBJ файле
    for (const auto& shape : shapes) {
        

        size_t index_offset = 0;
        for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {

            int fv = shape.mesh.num_face_vertices[f]; // Количество вершин на грань

            int material_id = shape.mesh.material_ids[f]; // Получение материала для текущей грани

            if (material_id < 0 || material_id >= materials.size()) { //проверка на существующий и допустимый материал
                continue;
            }

            const auto& material = materials[material_id];
            const GLuint textureID = textureMap[material.name];

            // Перебор вершин в текущей грани
            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shape.mesh.indices[index_offset + v];

                // Позиция
                vertices_coords[textureID].push_back(glm::vec3(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1], attrib.vertices[3 * idx.vertex_index + 2]));

                // Нормаль
                if (idx.normal_index >= 0) {
                    vertices_normals[textureID].push_back(glm::vec3(attrib.normals[3 * idx.normal_index + 0], attrib.normals[3 * idx.normal_index + 1], attrib.normals[3 * idx.normal_index + 2]));
                }

                // Текстурные координаты
                if (idx.texcoord_index >= 0) {
                    textures_coords[textureID].push_back(glm::vec2(attrib.texcoords[2 * idx.texcoord_index + 0], attrib.texcoords[2 * idx.texcoord_index + 1]));
                }
            }
            index_offset += fv;
        }
    }

    for (auto& pair : vertices_coords) { //формируем окончательный набор мешей
        const GLuint textureID = pair.first;

        Mesh mesh;

        mesh.textureID = pair.first;
        mesh.vertices_count = vertices_coords[textureID].size();

        glGenVertexArrays(1, &mesh.VAO);
        load_coords(vertices_coords[textureID].data(), vertices_coords[textureID].size(), mesh);
        load_normals(vertices_normals[textureID].data(), vertices_normals[textureID].size(), mesh);
        load_texcoord(textures_coords[textureID].data(), textures_coords[textureID].size(), mesh);

        meshes.push_back(mesh);
    }

    // загрузка шейдеров
    load_shaders("texvs.glsl", "texfs.glsl");
}

void SuperModel::render(glm::mat4 modelMatrix, glm::mat4 viewMatrix, glm::mat4 projMatrix, LightsInfo lights, GLuint mode)
{
    //Сначала всегда выбираем программу
    glUseProgram(shader_programme);

    //Теперь матрицы
    GLuint Matrix = glGetUniformLocation(shader_programme, "M");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &modelMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "V");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &viewMatrix[0][0]);
    Matrix = glGetUniformLocation(shader_programme, "P");
    glUniformMatrix4fv(Matrix, 1, GL_FALSE, &projMatrix[0][0]);

    //И свет... это долго
    //Сначала фоновый
    GLuint la = glGetUniformLocation(shader_programme, "lights.dirLight.ambient");
    glUniform3fv(la, 1, &lights.dirLight.ambient[0]);
    GLuint ld = glGetUniformLocation(shader_programme, "lights.dirLight.diffuse");
    glUniform3fv(ld, 1, &lights.dirLight.diffuse[0]);
    GLuint ls = glGetUniformLocation(shader_programme, "lights.dirLight.specular");
    glUniform3fv(ls, 1, &lights.dirLight.specular[0]);
    GLuint ldir = glGetUniformLocation(shader_programme, "lights.dirLight.direction");
    glUniform3fv(ldir, 1, &lights.dirLight.direction[0]);

    //Теперь точеные
    GLuint lnp = glGetUniformLocation(shader_programme, "lights.numPLights");
    glUniform1i(lnp, lights.numPLights);
    for (int i = 0; i < lights.numPLights; i++)
    {
        GLuint lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].ambient").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].ambient[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].diffuse").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].diffuse[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].specular").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].specular[0]);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].position").c_str());
        glUniform3fv(lp, 1, &lights.pointLights[i].position[0]);

        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].constant").c_str());
        glUniform1f(lp, lights.pointLights[i].constant);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].linear").c_str());
        glUniform1f(lp, lights.pointLights[i].linear);
        lp = glGetUniformLocation(shader_programme, ("lights.pointLights[" + to_string(i) + "].quadratic").c_str());
        glUniform1f(lp, lights.pointLights[i].quadratic);
    }

    //И направленные
    GLuint lns = glGetUniformLocation(shader_programme, "lights.numSLights");
    glUniform1i(lns, lights.numSLights);

    for (int i = 0; i < lights.numSLights; i++)
    {
        GLuint lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].ambient").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].ambient[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].diffuse").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].diffuse[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].specular").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].specular[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].position").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].position[0]);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].direction").c_str());
        glUniform3fv(lsp, 1, &lights.spotLights[i].direction[0]);


        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].cutOff").c_str());
        glUniform1f(lsp, lights.spotLights[i].cutOff);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].outerCutOff").c_str());
        glUniform1f(lsp, lights.spotLights[i].outerCutOff);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].constant").c_str());
        glUniform1f(lsp, lights.spotLights[i].constant);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].linear").c_str());
        glUniform1f(lsp, lights.spotLights[i].linear);
        lsp = glGetUniformLocation(shader_programme, ("lights.spotLights[" + to_string(i) + "].quadratic").c_str());
        glUniform1f(lsp, lights.spotLights[i].quadratic);
    }

    //А ещё нужны координаты камеры
    GLuint campos = glGetUniformLocation(shader_programme, "campos");
    glUniform3fv(campos, 1, &CamPosition[0]);

    for (const auto& mesh : meshes) {
        //Загрузка ссылки на текстуру в шейдер
        GLuint mtx = glGetUniformLocation(shader_programme, "material.texture");
        glUniform1i(mtx, 0);
        //Флаг есть ли карта отражений
        GLuint mm = glGetUniformLocation(shader_programme, "material.hasMap");
        //if (texturemap)
        //    glUniform1i(mm, 1);
        //else
        glUniform1i(mm, 0);

        GLuint ms = glGetUniformLocation(shader_programme, "material.specularmap");
        glUniform1i(ms, 1);

        //Активация текстур
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mesh.textureID);
        /*if (texturemap != -1)
        {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, texturemap);
        }*/

        //   И от материала всё равно нужен уровень зеркальности
        GLuint msh = glGetUniformLocation(shader_programme, "material.shininess");
        glUniform1f(msh, 64); //====================

        //А дальше всё так же как и в простом случае
        glBindVertexArray(mesh.VAO);
        glDrawArrays(mode, 0, mesh.vertices_count);
    }
}

GLuint SuperModel::LoadTexture(const std::string& filename)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Загрузка изображения и настройка параметров текстуры
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        GLenum format = (nrChannels == 3) ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        // Параметры фильтрации
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Настройка анизотропной фильтрации
        /*float maxAnisotropy;
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, maxAnisotropy);*/
    } else {
        std::cerr << "Failed to load texture: " << filename << std::endl;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

void SuperModel::load_coords(glm::vec3* verteces, size_t count, Mesh& mesh)
{
    //Создание VBO
    GLuint coords_vbo = 0;
    glGenBuffers(1, &coords_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, coords_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), verteces, GL_STATIC_DRAW);

    //Так как VAO уже создан, то можно сразу связать с ним
    glBindVertexArray(mesh.VAO);

    //Собственно связь
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    //Подключение атрибута
    glEnableVertexAttribArray(0);
}

void SuperModel::load_normals(glm::vec3* normals, size_t count, Mesh& mesh)
{
    GLuint normals_vbo = 0;
    glGenBuffers(1, &normals_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, normals_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec3), normals, GL_STATIC_DRAW);

    glBindVertexArray(mesh.VAO);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
}

void SuperModel::load_texcoord(glm::vec2* tex, size_t count, Mesh& mesh)
{
    GLuint tex_vbo = 0;
    glGenBuffers(1, &tex_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, tex_vbo);
    glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec2), tex, GL_STATIC_DRAW);

    glBindVertexArray(mesh.VAO);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(2);
}

void SuperModel::load_shaders(const char* vect, const char* frag)
{
    // Переменные под результат компиляции программы
    GLint result = GL_FALSE;
    int infoLogLength;

    //Создание шейдерной программы
    shader_programme = glCreateProgram();

    //Загрузка текстов шейдеров из файлов
    string vstext = LoadShader(vect);
    const char* vertex_shader = vstext.c_str();
    string fstext = LoadShader(frag);
    const char* fragment_shader = fstext.c_str();

    //Создание вершинного шейдера
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    //Проверка результата компиляции
    glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &infoLogLength);
    //Вывод сообщения об ошибке если что-то пошло не так
    if (infoLogLength > 0)
    {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(vs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    //Аналогично с фрагментным шейдером
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
    glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (infoLogLength > 0)
    {
        char* errorMessage = new char[infoLogLength + 1];
        glGetShaderInfoLog(fs, infoLogLength, NULL, errorMessage);
        std::cout << errorMessage;
        delete errorMessage;
    }

    //Сборка программы
    glAttachShader(shader_programme, vs);
    glAttachShader(shader_programme, fs);


    glBindAttribLocation(shader_programme, 0, "vertex_position");
    glBindAttribLocation(shader_programme, 1, "vertex_normal");
    glBindAttribLocation(shader_programme, 2, "vertex_texture");


    //Компоновка шейдерной программы
    glLinkProgram(shader_programme);
}


