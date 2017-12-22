#version 330 core

uniform mat4 myprojection_matrix;
uniform mat4 myview_matrix;
uniform mat4 mymodel_matrix;
uniform mat3 mynormal_matrix;

in vec4 myvertex;
in vec3 mynormal;
in vec2 mytexturecoordinate;

out vec4 color;

uniform sampler2D tex;

vec4 ConvolutionFilter(in int mode);

void main (void)
{   
	color = ConvolutionFilter(3);
	color = color + vec4(0.2,0.2,0.2,1);
	color.a = 1.0f;
}

vec4 get_pixel(in vec2 coords, in float dx, in float dy) 
{ 
   return texture(tex, coords + vec2(dx, dy));
}

float Convolve(in float[9] kernel, in float[9] matrix, in float denom, in float offset) 
{
   float res = 0.0;
   for (int i=0; i<9; i++) {
      res += kernel[i]*matrix[i];
   }
   return clamp(res/denom + offset,0.0,1.0);
}

float[9] GetData(in int channel) 
{
   float dxtex = 1.0 / float(textureSize(tex,0));  
   float dytex = 1.0 / float(textureSize(tex,0));
   float[9] mat;
   int k = -1;
   for (int i=-1; i<2; i++) {   
      for(int j=-1; j<2; j++) {    
         k++;    
         mat[k] = get_pixel(mytexturecoordinate.xy,float(i)*dxtex, float(j)*dytex)[channel];
      }
   }
   return mat;
}

float[9] GetMean(in float[9] matr, in float[9] matg, in float[9] matb) 
{
   float[9] mat;
   for (int i=0; i<9; i++) {
      mat[i] = (matr[i]+matg[i]+matb[i])/3.;
   }
   return mat;
}

vec4 ConvolutionFilter(in int mode)
{
   float[9] kerEmboss = float[] (2.,0.,0.,
                                 0., -1., 0.,
                                 0., 0., -1.);

   float[9] kerSharpness = float[] (-1.,-1.,-1.,
                                    -1., 9., -1.,
                                    -1., -1., -1.);

   float[9] kerGausBlur = float[]  (1.,2.,1.,
                                    2., 4., 2.,
                                    1., 2., 1.);

   float[9] kerEdgeDetect = float[] (-1./8.,-1./8.,-1./8.,
                                     -1./8., 1., -1./8.,
                                     -1./8., -1./8., -1./8.);

   float matr[9] = GetData(0);
   float matg[9] = GetData(1);
   float matb[9] = GetData(2);
   float mata[9] = GetMean(matr,matg,matb);

   // Sharpness kernel
   if (mode == 1)
   return vec4(Convolve(kerSharpness,matr,1.,0.),
                       Convolve(kerSharpness,matg,1.,0.),
                       Convolve(kerSharpness,matb,1.,0.),1.0);

   // Gaussian blur kernel
   if (mode == 2)
   return vec4(Convolve(kerGausBlur,matr,16.,0.),
                       Convolve(kerGausBlur,matg,16.,0.),
                       Convolve(kerGausBlur,matb,16.,0.),1.0);

   // Edge Detection kernel
   if (mode == 3)
   return vec4(Convolve(kerEdgeDetect,mata,0.1,0.),
                       Convolve(kerEdgeDetect,mata,0.1,0.),
                       Convolve(kerEdgeDetect,mata,0.1,0.),1.0);

   // Emboss kernel
   return vec4(Convolve(kerEmboss,mata,1.,1./2.),
                       Convolve(kerEmboss,mata,1.,1./2.),
                       Convolve(kerEmboss,mata,1.,1./2.),1.0);

}