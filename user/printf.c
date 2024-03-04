#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#include <stdarg.h>

static char digits[] = "0123456789ABCDEF";

static void
putc(int fd, char c)
{
  write(fd, &c, 1);
}

static void
printint(int fd, int xx, int base, int sgn)
{
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0)
    putc(fd, buf[i]);
}

void
sprint_printint(char *strbuf, int xx, int base, int sgn)
{
  char buf[16];
  int i, neg;
  uint x;

  neg = 0;
  if(sgn && xx < 0){
    neg = 1;
    x = -xx;
  } else {
    x = xx;
  }

  i = 0;
  do{
    buf[i++] = digits[x % base];
  }while((x /= base) != 0);
  if(neg)
    buf[i++] = '-';

  while(--i >= 0){
  	*strbuf++ = buf[i]; 
  	}
    //putc(fd, buf[i]);

  *strbuf = '\0'; 
}


static void
printptr(int fd, uint64 x) {
  int i;
  putc(fd, '0');
  putc(fd, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    putc(fd, digits[x >> (sizeof(uint64) * 8 - 4)]);
}

void
sprintptr(char *buf, uint64 x) {
  int i;

  *buf++ = '0';
  *buf++ = 'x';
  //putc(fd, '0');
  //putc(fd, 'x');
  for (i = 0; i < (sizeof(uint64) * 2); i++, x <<= 4)
    buf[i] = digits[x >> (sizeof(uint64) * 8 - 4)];

  *buf = '\0';
}



// Print to the given fd. Only understands %d, %x, %p, %s.
void
vprintf(int fd, const char *fmt, va_list ap)
{
  char *s;
  int c, i, state;

  state = 0;
  for(i = 0; fmt[i]; i++){
    c = fmt[i] & 0xff;
    if(state == 0){
      if(c == '%'){
        state = '%';
      } else {
        putc(fd, c);
      }
    } else if(state == '%'){
      if(c == 'd'){
        printint(fd, va_arg(ap, int), 10, 1);
      } else if(c == 'l') {
        printint(fd, va_arg(ap, uint64), 10, 0);
      } else if(c == 'x') {
        printint(fd, va_arg(ap, int), 16, 0);
      } else if(c == 'p') {
        printptr(fd, va_arg(ap, uint64));
      } else if(c == 's'){
        s = va_arg(ap, char*);
        if(s == 0)
          s = "(null)";
        while(*s != 0){
          putc(fd, *s);
          s++;
        }
      } else if(c == 'c'){
        putc(fd, va_arg(ap, uint));
      } else if(c == '%'){
        putc(fd, c);
      } else {
        // Unknown % sequence.  Print it to draw attention.
        putc(fd, '%');
        putc(fd, c);
      }
      state = 0;
    }
  }
}

/**
This handles the basic sprintf() function
@param fd the fd to write to
@param buf the string buffer to write into
@param 
**/
void
vsprintf(int fd, char *buf, const char *fmt, va_list ap)
{

	char *s;
  	int c, i, state;

  	char *buffer = buf; // need to write it somewhere
	// basically where i see puts, I am going to remove and write to the buffer 
	state = 0;
	  for(i = 0; fmt[i]; i++){
	    c = fmt[i] & 0xff;
	    if(state == 0){
	      if(c == '%'){
	        state = '%';
	      } else {
	        *buffer++ = c;
	      }
	} else if(state == '%'){
	      if(c == 'd'){
	        sprint_printint(buffer, va_arg(ap, int), 10, 1);
	      } else if(c == 'l') {
	        sprint_printint(buffer, va_arg(ap, uint64), 10, 0);
	      } else if(c == 'x') {
	        sprint_printint(buffer, va_arg(ap, int), 16, 0);
	      } else if(c == 'p') {
	        sprintptr(buffer, va_arg(ap, uint64));
	      } else if(c == 's'){
	        s = va_arg(ap, char*);
	        if(s == 0)
	          s = "(null)";
	        while(*s != 0){
	          *buffer++ = *s++; 
	        }
	} else if(c == 'c'){
	        *buffer++ = (char)va_arg(ap, uint);
	      } else if(c == '%'){
	        *buffer++ = c; 
	      } else {
	        // Unknown % sequence.  Print it to draw attention.
	       	*buffer++ = '%'; 
	        //putc(fd, '%');
	       // putc(fd, c);
	       *buffer++ = c; 
	      }
	      state = 0;
	    }
	  }
  *buffer = '\0'; // have to null terminate 
}


/**
The function that handles memory safe sprintf()
@param fd the fd to write to
@param buf the buffer to write the string into
@para size the max amount of bytes to write to buf
**/
void
vsnprintf(int fd, char *buf, int size, const char *fmt, va_list ap)
{
  	char *s;
  	int c, i, state;

	char *buffer = buf; // need to write it somewhere
	// basically where i see puts, I am going to remove and write to the buffer 
	state = 0;
	int count = 0; 
	while (count < size){
	  for(i = 0; fmt[i]; i++){
	    c = fmt[i] & 0xff;
	    if(state == 0){
	      if(c == '%'){
	        state = '%';
		  } else {
		 	*buffer++ = c;
        	count++;
	      }
	    } else if(state == '%'){
	      if(c == 'd'){
	        sprint_printint(buffer, va_arg(ap, int), 10, 1);
	      } else if(c == 'l') {
	        sprint_printint(buffer, va_arg(ap, uint64), 10, 0);
	      } else if(c == 'x') {
	        sprint_printint(buffer, va_arg(ap, int), 16, 0);
	      } else if(c == 'p') {
	        sprintptr(buffer, va_arg(ap, uint64));
		} else if(c == 's'){
	        s = va_arg(ap, char*);
	        if(s == 0)
	          s = "(null)";
	        while(*s != 0){
	          *buffer++ = *s++; 
	          count++; 
	        }
	      } else if(c == 'c'){
	        *buffer++ = (char)va_arg(ap, uint);
	        count++; 
	      } else if(c == '%'){
	        *buffer++ = c; 
	        count++; 
	      } else {
	        // Unknown % sequence.  Print it to draw attention.
	       	*buffer++ = '%'; 
	        //putc(fd, '%');
	       // putc(fd, c);
	       *buffer++ = c; 
	       count += 2; 
	      }
	      state = 0;
	    }
	   }
	  if (count >= size){
	  	break;
	  }
	}
	  buffer[size] = '\0'; // have to null terminate 
}

void
fprintf(int fd, const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(fd, fmt, ap);
}

void
printf(const char *fmt, ...)
{
  va_list ap;

  va_start(ap, fmt);
  vprintf(1, fmt, ap);
}

/**
writes into a buffer 
* @param strbuf the buffer to write into
* @return returns the number of characters written into the buffer
**/
int 
sprintf(char* strbuf, const char *format, ...)
{
	va_list ap; 
	va_start(ap, format); 
	// has to return an int, place holding for now 
	
	vsprintf(1, strbuf, format, ap);

 	int length = 0; 
 	for (int i = 0 ; strbuf[i] != '\0'; i++){
  		length++; 
  	}
	return length; // need to return the number of bytes read 
}

/**
str is the buffer to write into, returns the number of bytes read
not including the null terminator 
n is the size of the buffer to write into 
* @param str the buffer to write into 
* @param maxiumum number of bytes to be written to buffer 
* @return returns the total number of characters written to buffer
**/
int
snprintf(char *str, int n, const char *format, ...)
{
	 // allocating the space of the buffer to write into 
	
	va_list ap; 
	va_start(ap, format); 

	vsnprintf(1, str,n, format, ap); 
	// properly terminating the buffer if it overwrites the size
	int size = strlen(str); 
	if (size >= n){
		str[n] = '\0';
	}

	// does not include the null terminator 
	// returns the number of bytes read
	return strlen(str) -1; 
}
