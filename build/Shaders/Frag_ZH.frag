#version 430

// pipeline-ból bejövő per-fragment attribútumok
in vec3 vs_out_pos;
in vec3 vs_out_norm;
in vec2 vs_out_tex;

// kimenő érték - a fragment színe
out vec4 fs_out_col;

// textúra mintavételező objektum
const int SHADER_STATE_OCEAN = 0;
const int SHADER_STATE_DEFAULT = 1;
const int SHADER_STATE_OCEAN_SURFACE = 2;

uniform int state;


uniform sampler2D texImage;
uniform float m_ElapsedTimeInSec;
uniform vec4 lightPos = vec4( 0.0, 1.0, 0.0, 0.0);
uniform vec4 lightPos2 = vec4( 0.0, 1.0, 0.0, 1.0);
uniform vec3 cameraPos;


uniform vec3 La = vec3(0.0, 0.0, 0.0 );
uniform vec3 Ld = vec3(1.0, 1.0, 1.0 );
uniform vec3 Ls = vec3(1.0, 1.0, 1.0 );

uniform float lightConstantAttenuation    = 1.0;
uniform float lightLinearAttenuation      = 0.0;
uniform float lightQuadraticAttenuation   = 0.0;

uniform bool enableRedLight;

// anyag tulajdonsagok

uniform vec3 Ka = vec3( 1.0 );
uniform vec3 Kd = vec3( 1.0 );
uniform vec3 Ks = vec3( 1.0 );

uniform vec3 lightColorMultiplier = vec3(1.0);
uniform vec3 darkColorMultiplier = vec3(0.8,0.8,0.9);

uniform float Shininess = 1.0;

struct LightProperties{
	vec4 pos;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};

vec3 redPointLight(vec3 fragPos)
{
    vec3 N = normalize(vs_out_norm);


    vec3 L = normalize(lightPos2.xyz - fragPos);

    float NdotL = max(dot(N, L), 0.0);

    return vec3(1.0, 0.0, 0.0) * NdotL;
}


vec3 lighting(LightProperties light){
	vec3 normal = normalize( vs_out_norm );
	
	vec3 ToLight; 
	float LightDistance = 0.0; 
	
	if ( light.pos.w == 0.0 )
	{
		ToLight	= light.pos.xyz;
	}
	else
	{
		ToLight	= light.pos.xyz - vs_out_pos;
		LightDistance = length(ToLight);
	}
	ToLight = normalize(ToLight);
	float Attenuation = 1.0 / ( light.constantAttenuation + light.linearAttenuation * LightDistance + light.quadraticAttenuation * LightDistance * LightDistance);

	vec3 Ambient = light.La * Ka;
	float DiffuseFactor = max(dot(ToLight,normal), 0.0) * Attenuation;
	vec3 Diffuse = DiffuseFactor * light.Ld * Kd;
	vec3 viewDir = normalize( cameraPos - vs_out_pos ); // A fragmentből a kamerába mutató vektor
	vec3 reflectDir = reflect( -ToLight, normal ); // Tökéletes visszaverődés vektora
	
	float FragShininess = Shininess;

	
	float SpecularFactor = pow(max( dot( viewDir, reflectDir) ,0.0), FragShininess) * Attenuation;
	
	vec3 Specular = SpecularFactor * light.Ls * Ks;

	return Ambient + Diffuse + Specular;
}


void main()
{
    vec4 texColor = texture(texImage, vs_out_tex);

    if(state == SHADER_STATE_OCEAN){
        fs_out_col = texColor;
    }

    if(state == SHADER_STATE_OCEAN_SURFACE){
        vec2 uv = vs_out_tex + vec2(m_ElapsedTimeInSec, m_ElapsedTimeInSec) / 150.0;
        fs_out_col = texture(texImage, uv);
    }

    if(state == SHADER_STATE_DEFAULT){
        LightProperties light;
        light.pos = lightPos;
        light.La = La;
        light.Ld = Ld;
        light.Ls = Ls;
        light.constantAttenuation = lightConstantAttenuation;
        light.linearAttenuation = lightLinearAttenuation;
        light.quadraticAttenuation = lightQuadraticAttenuation;

        vec3 shadedColor = lighting(light);
        fs_out_col = vec4(shadedColor, 1.0) * texColor;
    }
    float y = vs_out_pos.y;
    vec3 coeff = vec3(0.014, 0.01, 0.004);
    vec3 absorb = exp(coeff * min(0.0, y));
    fs_out_col *= vec4(absorb, 1.0);

    vec3 redAdd = vec3(0.0);
    if (enableRedLight) {
        redAdd = redPointLight(vs_out_pos);
    }

    fs_out_col += vec4(redAdd, 1.0);
}
