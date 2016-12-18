#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define CHARS 6
int DEBUG; 
#define MAX(a,b) (a>b?a:b)
#define MIN(a,b) (a<b?a:b)

int puzzle(int off,int cbyte,unsigned char *xbuf,unsigned char *buf,int buflen,int t,int p){
DEBUG && fprintf(stderr,"BAR %u\n",off);
  int offset = t?((buf[off]&0xf0)>>4)&0xf:(buf[off] &0xf) % buflen;
DEBUG && fprintf(stderr,"FOO %u\n",offset);
  unsigned char j=8*(cbyte-1);
  unsigned int num = 0;
  for(int i =0;i<cbyte;i++,j-=8){
    int next = (unsigned int)(p?((buf[offset+i]&(unsigned char)0xff)^(xbuf[i]&(unsigned char)0xff))<<j
              :(buf[offset+i]&xbuf[i])<<j);
DEBUG && fprintf(stderr,"BAZ %u %u %u %u\n",offset,i,j,next);
    num |= next;
  }
  num %= (unsigned int)pow(10,CHARS);
  return num;
}

int hotp(char *code,unsigned char *buf,int buflen){
  if(!buflen)
    return 0;
  unsigned char xbuf[5] = {0x7f,0xff,0xff,0xff,0xff};
  int num = puzzle(buflen-1,4,xbuf,buf,buflen,0,0);
  sprintf (code,"%0*u",CHARS,num);
  return 1;
}

int c; 
int totp(char *code,unsigned char *secret,int div){
  if(DEBUG==1){
    unsigned char hmac[20] = {0x1f,0x86,0x98,0x69,0x0e,0x02,0xca,0x16,0x61,0x85,0x50,0xef,0x7f,0x19,0xda,0x8e,0x94,0x5b,0x55,0x5a};
    hotp(code,hmac,20);
    printf("%s\n",code);
  }
  unsigned char hmac[512];
  int hmaclen;
  char szd[12];
  unsigned int t = DEBUG>1?c:(unsigned)floor(time(0)/div);
	DEBUG && (c += 5);
  snprintf(szd,sizeof(szd),"%u",t);
DEBUG && fprintf(stderr,"%s\n%s\n",secret,szd);
  HMAC(EVP_sha1(),secret,strlen(secret)-1,szd,strlen(szd),hmac,&hmaclen);
  for(int i=(DEBUG?0:hmaclen);i<hmaclen;i++){
    fprintf(stderr,"%u,",hmac[i]);
  }
DEBUG && fprintf(stderr,"\n%u\n",hmaclen);
  return hotp(code,hmac,hmaclen);
}
char *fsecret = "./.lob-secret.txt";
int main(int argc,char *argv[]){
  DEBUG = argc>1?atoi(argv[1]):0;
	if(DEBUG>1){
		unsigned int seed;
		FILE* random = fopen("/dev/random", "r");
		fseek(random,trunc(time(0)/(24*3600)),SEEK_CUR);	
		fread(&seed, sizeof(int), 1, random);
		fclose(random);
		srand(seed);
		c = rand();
		fprintf(stderr,"RAND? %u\n",c);
	}
  unsigned char *secret;
	struct stat sb;
	char spath[256];
	do{
		if(argc>2){
			if(stat(argv[2],&sb)){
				secret = argv[2];
				break;
			}else{
				sprintf(spath,argv[2]);			
			}
		}else{
			snprintf(spath,256,"%s/%s",getenv("HOME"),fsecret);
		}
	  FILE *fd = fopen(spath,"r");
	  stat(spath,&sb);
	  secret = malloc(sb.st_size+1);
		int len;
		do{
			if(13>sb.st_size){
				secret[len=fread(secret,1,sb.st_size,fd)] = 0;
				if(secret[len-1]=='\n')secret[len-1]=0;
				break;
			}
			fread(secret,1,13,fd);
		  if(0==strncmp(secret,"BASE64SECRET\n",13)){
				BIO *bio, *b64, *bio_out;
  	    char inbuf[2048];
    	  int inlen;
 	     	b64 = BIO_new(BIO_f_base64());
  	    bio = BIO_new_fp(fd, BIO_NOCLOSE);
	      BIO_push(b64, bio);
  	    inbuf[inlen = BIO_read(b64, inbuf, 2048)] = 0;
				secret = inbuf;
				break;
			}
	 		secret[len=13+fread(secret+13,1,sb.st_size-13,fd)] = 0;
			if(secret[len-1]=='\n')secret[len-1]=0;
		}while(0);
	}while(0);
  char code[CHARS+1];
	if(DEBUG>1){
		for(int i=0;i<DEBUG;i++){
			totp(code,secret,1);
			printf("%s",code);
		}
	}
  if(!totp(code,secret,30))
    printf("Blargle\n");
  else{
    printf(code);
    if(isatty(1))
      printf("\n");
  }
}
