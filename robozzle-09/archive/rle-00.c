#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc,char *argv[]) {

	int w,h,x,y,d;

	int c=-1;
	int p=-1;
	int f=0;

	if(argc!=3) {
		fprintf(stderr,"syntax: %s LevFile EncFile\n",argv[0]);
		return 1;
	}

	FILE *fp0=fopen(argv[1],"r");

	if(!fp0) {
		fprintf(stderr,"Error opening file %s: %s\n",argv[1],strerror(errno));
		return 1;
	}

	FILE *fp1=fopen(argv[2],"w");

	if(!fp1) {
		fprintf(stderr,"Error opening file %s: %s\n",argv[2],strerror(errno));
		return 1;
	}

	fscanf(fp0,"%d,%d,%d,%d,%d",&w,&h,&x,&y,&d);	fprintf(fp1,"%d,%d,%d,%d,%d,",w,h,x,y,d);

	f=0;
	while((c=fgetc(fp0))!=EOF) {
		if(c>='0' && c<='7') {
			if(c==p) {
				f++;
			} else if(f==0) {
				f=1;
				p=c;
			} else if(f==1) {
				fprintf(fp1,"%c",p-'0'+'G');
				f=1;
				p=c;
			} else if(f==2) {
				fprintf(fp1,"%c%c",p-'0'+'G',p-'0'+'G');
				f=1;
				p=c;
			} else if(f>2) {
				fprintf(fp1,"%X%c",f,p-'0'+'G');
				f=1;
				p=c;
			}
		}
	}
	if(f==1) {
		fprintf(fp1,"%c",p-'0'+'G');
		f=1;
		p=c;
	} else if(f==2) {
		fprintf(fp1,"%c%c",p-'0'+'G',p-'0'+'G');
		f=1;
		p=c;
	} else if(f>2) {
		fprintf(fp1,"%X%c",f,p-'0'+'G');
		f=1;
		p=c;
	}
	fprintf(fp1,"\n");
	fclose(fp1);
	fclose(fp0);

	return 0;
}
