#include <stdio.h>
#include <math.h>
#include <time.h>

/* This file is the property of the University of Alabama in Huntsville's
Computer Science Department.  Usage or re-transmission prohibited without 
approval

Tim Newman  Feb. 1999 */
	
FILE *data_in,  /* raw data set */
 	*data_out;  /* results */

float convert(float degrees)    /* The variable angle_degrees, passed by
                                ** main(), is renamed to degrees here */
{   /* start of convert() */
 
   /* Variable declarations */
   float pi = 3.1415927;        /* pi to 7 decimal places */
   float radians;               /* the angle in radians */
 
 
   /* Calculation */
   radians = degrees*pi/180.0;    /* formula to convert degrees to radians */
 
   /* Return answer to calling function */
   return radians;
 
}   /* end of convert */


void main(){
    void *ptr;
    double rotation[3][3];
    unsigned char volume[72][256][256];
    unsigned char image_2d[256][256];	
    unsigned char intensity;
    unsigned char pixel;
    unsigned char material=0;         	
    float x_grad, y_grad, z_grad, gradient;
    int x, y, z, x_ind, y_ind;
    int i, j ,k;
    int pix_int;
    float rot_x=0;
    float rot_y=0;
    float rot_z=0;
    float x_rot, y_rot, z_rot;
    clock_t start, end;
    double elapsed;
    FILE *fp;

    fp = fopen("rot.dat","r");	
    fprintf(fp, "Enter X axis rotation:");
    fscanf(fp,"%f", &rot_x);
	rot_x=convert(rot_x);
	printf("Enter Y axis rotation:");
	fscanf(fp,"%f", &rot_y);
	rot_y=convert(rot_y);
    fclose(fp);

	
    /************** calculate rotation matrix ********/
    rotation[0][0]=cos(rot_z)*cos(rot_y);
    rotation[0][1]=sin(rot_z)*cos(rot_x)+cos(rot_z)*sin(rot_y)*sin(rot_x);
    rotation[0][2]=sin(rot_z)*sin(rot_x)-cos(rot_z)*sin(rot_y)*cos(rot_x);
    rotation[1][0]=-sin(rot_z)*cos(rot_y);
    rotation[1][1]=cos(rot_z)*cos(rot_x)-sin(rot_z)*sin(rot_y)*sin(rot_x);
    rotation[1][2]=cos(rot_z)*sin(rot_x)+sin(rot_z)*sin(rot_y)*cos(rot_x);
    rotation[2][0]=sin(rot_y);
    rotation[2][1]=-cos(rot_y)*sin(rot_x);
    rotation[2][2]=cos(rot_y)*cos(rot_x);	

	/*** clear data  ***/
	for(x=0; x<256; x++){
		for(y=0; y<256; y++){
			image_2d[x][y]=0;
		}
	}	
	/************** opening input data file ************/
	data_in=fopen("mra-data", "rb" );
	if( data_in == NULL )
		printf( "The file mra-data was not opened\n" );
	else{

	/************** opening output data file ***********/
		data_out=fopen("nusplat.GRAY", "w+b");
		if( data_out == NULL )
			printf( "The file nusplat.GRAY was not opened\n" );
		else{
			fseek(data_in, 0L, SEEK_SET);  /*go to start of file*/
			start = clock();
			end = clock();
			elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
			printf("clock takes...%f\n", elapsed);
			
            /**** reading in the grid *******/
			printf("reading in data...\n");
			for(i=0; i<=71; i++){
				for(j=0; j<=255;j++){
				   fread(&volume[i][j],sizeof(unsigned char), 256, data_in); 
				}}
			
			
			start=clock();
                        for(i=0; i<=71; i++){
                                for(j=0; j<=255;j++){
                                        for(k=0;k<=255;k++){
				/********multiply by rotation matrix ********/
					  x_rot=rotation[0][0]*(i-36)+
				            rotation[0][1]*(j-128)+
					    rotation[0][2]*(k-128);
					  x=(floor) (x_rot);
                      x_ind=x+128;
					  y_rot=rotation[1][0]*(i-36)+	
					        rotation[1][1]*(j-128)+
					        rotation[1][2]*(k-128);
					  y=(floor)(y_rot);	
                      y_ind=y+128;   
					  z_rot=rotation[2][0]*(i-36)+	
					        rotation[2][1]*(j-128)+
					  rotation[2][2]*(k-128);	
					  z=(floor)(z_rot);
                                               
			/****** store in image array *******************/
                      if(x<128&&x>=-128&&y<128&&y>=-128){ 
     if(k>0 && k<255 && i>0 && i<71 && j>0 &&j<255){
        x_grad=(volume[i+1][j][k]-volume[i-1][j][k]) / 2;
        y_grad=(volume[i][j+1][k]-volume[i][j-1][k]) / 2;
        z_grad=(volume[i][j][k+1]-volume[i][j][k-1]) / 2;
        gradient=sqrt(((double)x_grad)*x_grad+((double)y_grad)*y_grad+((double) z_grad)*z_grad);
   } 
image_2d[x_ind][y_ind]+=floor((1.0-fabs(x_rot-x)*fabs(y_rot-y))*gradient/4.0);
image_2d[x_ind+1][y_ind]+=floor((1.0-fabs(x_rot-x-1.0)*fabs(y_rot-y))*gradient/4.0);
image_2d[x_ind][y_ind+1]+=floor((1.0-fabs(x_rot-x)*fabs(y_rot-y-1.0))*gradient/4.0);
image_2d[x_ind+1][y_ind+1]+=floor((1.0-fabs(x_rot-x-1.0)*fabs(y_rot-y-1.0))*gradient/4.0);
					  }
  					}
				    }
				}
			
			end = clock();
			elapsed = ((double) (end - start)) / CLOCKS_PER_SEC;
	
		for(x=0; x<256; x++){
                    for(y=0; y<256;y++){
			fprintf(data_out, "%c", image_2d[x][y]);
		}}

   fclose(data_in);   /** closing the files **/
   } /** end second else **/   
	
fclose(data_out); 	
}/** end first else **/
	
printf("CPU time: %.2f\n", elapsed);	
}/*end main */
 	
