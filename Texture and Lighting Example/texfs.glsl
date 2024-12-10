#version 400

#define MAX_NUM_LIGHTS 10

in vec3 normal;
in vec3 f_pos;
in vec2 TexCoord;

uniform	vec3 campos;

struct Material {
	sampler2D texture;
	sampler2D specularmap;
	float shininess;
	int hasMap;
};

uniform Material material;

struct DirLight
{
	vec3 direction;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct PointLight
{
	vec3 position;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};


struct LightsInfo {

	DirLight dirLight;
	PointLight pointLights[MAX_NUM_LIGHTS];
	SpotLight spotLights[MAX_NUM_LIGHTS];

	int numPLights;
	int numSLights;

};

uniform LightsInfo lights;

out vec4 frag_color;


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 specular;
	//Нормализация направления света
	vec3 lightDir = normalize(-light.direction);
	// Диффузный коэффициент
	float diff = max(dot(normal, lightDir), 0.0);
	// Отражённый коэффициент
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Результирующие компоненты с учётом материала
	vec3 ambient = light.ambient * vec3(texture(material.texture, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture, TexCoord));
	if (material.hasMap > 0)
		specular = light.specular * spec * vec3(texture(material.specularmap, TexCoord));
	else
		specular = light.specular * spec * vec3(0, 0, 0);
	return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 specular;
	//Вычисление вектора света
	vec3 lightDir = normalize(light.position - fragPos);
	// Диффузный коэффициент
	float diff = max(dot(normal, lightDir), 0.0);
	//  Отражённый коэффициент
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// Затухание
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance +
		light.quadratic * (distance * distance));
	// Результирующие компоненты с учётом материала
	vec3 ambient = light.ambient * vec3(texture(material.texture, TexCoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture, TexCoord));
	if (material.hasMap > 0)
		specular = light.specular * spec * vec3(texture(material.specularmap, TexCoord));
	else
		specular = light.specular * spec * vec3(0, 0, 0);
	//Влияние затухания
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;
	return (ambient + diffuse + specular);
}


vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 specular;
	//Вычисление вектора света
	vec3 lightDir = normalize(light.position - fragPos);

	vec3 result;

	//Вычисление косинуса угла к фрагменту
	float theta = dot(lightDir, normalize(-light.direction));

	//Так как угол передан в виде косинуса, найденный угол дожен быть больше заданного
	if (theta > light.cutOff)
	{
		// Фоновая компонента
		vec3 ambient = light.ambient * vec3(texture(material.texture, TexCoord));

		// Диффузная компонента 
		vec3 norm = normalize(normal);
		float diff = max(dot(norm, lightDir), 0.0);
		vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture, TexCoord));

		// Отражённая компонента
		vec3 viewDir = normalize(campos - fragPos);
		vec3 reflectDir = reflect(-lightDir, norm);
		float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
		if (material.hasMap > 0)
			specular = light.specular * spec * vec3(texture(material.specularmap, TexCoord));
		else
			specular = light.specular * spec * vec3(0, 0, 0);
		// Затухание
		float distance = length(light.position - fragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

		// Затухание не должно влиять на фоновую компоненту
		diffuse *= attenuation;
		specular *= attenuation;

		result = ambient + diffuse + specular;
	}
	else
	{
		// Если фрагмен не в конусе - только фоновая компонента оказывает влияние на результат
		result = light.ambient * vec3(texture(material.texture, TexCoord));
	}

	return result;
}



void main() {


	//Нормализация вектора нормали
	vec3 norm = normalize(normal);

	//Определение направления взгляда
	vec3 viewDir = normalize(campos - f_pos);

	//Учёт направленного фонового освещения
	vec3 result = CalcDirLight(lights.dirLight, norm, viewDir);

	//Учёт точечных источников света
	for (int i = 0; i < lights.numPLights; i++)
		result += CalcPointLight(lights.pointLights[i], norm, f_pos, viewDir);

	//Учёт направленных источников света
	for (int i = 0; i < lights.numSLights; i++)
		result += CalcSpotLight(lights.spotLights[i], norm, f_pos, viewDir);

	//Итоговый цвет фрагмента
	frag_color = vec4(result, 1.0);

}