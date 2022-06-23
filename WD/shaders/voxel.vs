#version 460 core

const int _MaxPalette = 16;

layout(location = 0) in uint a_data;

flat out struct {
	vec3 position;
	vec3 color;
	int neighbours;
} vs_out;

uniform vec3 _ColorPalette[_MaxPalette];

void main()
{
	vec3 position;
	position.x = float(bitfieldExtract(a_data, 0, 4));
	position.y = float(bitfieldExtract(a_data, 4, 4));
	position.z = float(bitfieldExtract(a_data, 8, 4));

	vec3 color = vec3(1.0f, 0.0f, 1.0f);

	int colorIndex = int(bitfieldExtract(a_data, 12, 4));
	int neighbours = int(bitfieldExtract(a_data, 16, 6));

	switch(colorIndex)
	{
		default:
		case 0: color = _ColorPalette[0]; break;
		case 1: color = _ColorPalette[1]; break;
		case 2: color = _ColorPalette[2]; break;
		case 3: color = _ColorPalette[3]; break;
		case 4: color = _ColorPalette[4]; break;
		case 5: color = _ColorPalette[5]; break;
		case 6: color = _ColorPalette[6]; break;
		case 7: color = _ColorPalette[7]; break;
		case 8: color = _ColorPalette[8]; break;
		case 9: color = _ColorPalette[9]; break;
		case 10: color = _ColorPalette[10]; break;
		case 11: color = _ColorPalette[11]; break;
		case 12: color = _ColorPalette[12]; break;
		case 13: color = _ColorPalette[13]; break;
		case 14: color = _ColorPalette[14]; break;
		case 15: color = _ColorPalette[15]; break;
	}

	vs_out.position = position;
	vs_out.color = color;
	vs_out.neighbours = neighbours;
}
