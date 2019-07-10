#define VERTEX_UI
#pragma include inc/mylly.glinc

varying vec2 texCoord;

#if defined(VERTEX_SHADER)

void main()
{
	gl_Position = vec4(Vertex, 0, 1);
	texCoord = TexCoord;
}

#elif defined(FRAGMENT_SHADER)

uniform int BloomSamples; // 5
uniform float BloomQuality; // 2.5
uniform float BloomFactor; // 1.0

void main()
{
	vec4 source = texture(TextureMain(), texCoord);

	// Calculate a sum of fragment colours around the current fragment.
	vec4 sum = vec4(0);
	int diff = (BloomSamples - 1) / 2;
	vec2 sizeFactor = vec2(BloomQuality) / ScreenResolution();

	for (int x = -diff; x <= diff; x++) {

		for (int y = -diff; y <= diff; y++) {

			vec2 offset = vec2(x, y) * sizeFactor;
			sum += texture(TextureMain(), texCoord + offset);
		}
	}

	vec4 bloom = BloomFactor * (sum / (BloomSamples * BloomSamples));

	gl_FragColor = (bloom + source);
}

#endif
