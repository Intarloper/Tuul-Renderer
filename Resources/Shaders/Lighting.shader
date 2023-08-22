#shader vertex

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNorm;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform float time;

out vec3 normal;
out vec3 FragPos;

float CalculateWaveDisplacement(vec4 modelPos){
	const float e = 2.718281828459;
	
	
	float displace = 0.0;
	float diagonalWaveAmp = .1;
	float xyWaveAmp = .01;
	float diagonalResult;
	float xyWaveResult;

	for(int i = 1; i < 4; i++){
		float yWave =  pow(e, (i * .3) * sin(((modelPos.y - modelPos.x) * i) + time));
		float xWave =  pow(e, (i * .5) * sin(((modelPos.x - modelPos.y) * i) + time * (i * .2)));
		float zWave =  pow(e, (i * .2) * sin((((modelPos.z - modelPos.y) * i) + ((modelPos.x - modelPos.y) * i)) + time * (i * .3)));
		
		yWave += (i - 1) * (pow(e, (i * .3) * sin(((modelPos.y - modelPos.x) * i) + time)) * ((i * .3) * cos((modelPos.y - modelPos.x) * i)));
		xWave += (i - 1) * (pow(e, (i * .5) * sin(((modelPos.x - modelPos.y) * i) + time)) * ((i * .5) * cos((modelPos.x - modelPos.y) * i)));
		
		diagonalResult = diagonalWaveAmp * zWave;
		xyWaveResult = xyWaveAmp * (yWave + xWave);

		displace = displace + xyWaveResult + diagonalResult;
	};
	return displace;
};

void main()
{	

	vec4 modelPos = model * vec4(aPos.xyz, 1.0);	
	float displace = CalculateWaveDisplacement(modelPos);

        float xDisplace = .5 * sin(modelPos.x + time * .1);
	float zDisplace = .5 * sin(modelPos.z + time * .13);
	

	normal = mat3(transpose(inverse(model))) * aNorm;	
	gl_Position = proj * view * model * vec4( aPos.x + xDisplace , aPos.y + displace, aPos.z + zDisplace, 1.0);
	FragPos = vec3(model * vec4(aPos, 1.0));
	//normal = aNorm;
};	

#shader fragment

#version 330 core

in vec3 normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float specValue;
uniform float ambientIntensity;

struct PointLight {
	vec3 position;
	vec3 color;

	float constant;
	float linear;
	float quadratic;
};


uniform PointLight light;

uniform bool lightType;
uniform bool useBlinn;

void main()
{
	//Lighiting Section
	//ambient
	vec3 ambientValue = ambientIntensity * lightColor;
	vec3 ambientResult = ambientValue * objectColor;

	//diffuse
	vec3 lightDirection = normalize( lightPos - FragPos);
	vec3 norm = normalize(normal);
	float diff = max(dot(lightDirection, norm), 0.0);
	vec3 diffuse = diff * lightColor;
	//specular
	
	float specStrength = 0.5;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDirection, norm);
	vec3 halfwayDir = normalize(lightDirection + viewDir);
	
	float spec = 0.0;
	if(useBlinn){
		spec = pow(max(dot(norm, halfwayDir), 0.0), specValue);
	}
	else{
		spec = pow(max(dot(viewDir, reflectDir), 0.0), specValue);
	}

	vec3 specular =  spec * lightColor;
	
	//point light calc + implementation
	if(lightType){
		float distance = length(light.position - FragPos);
		float attenuation = 1.0 / (light.constant + light.linear * distance + 
					light.quadratic * (distance * distance));
	
		ambientResult *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
	};
	
	vec3 result = (ambientResult + diffuse + specular) * objectColor;
	
	//FragColor = vec4(result, 1.0f);
	FragColor = vec4(result, 1.0f);
};

