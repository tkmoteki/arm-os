/* private interface */
#include "ctype.h"

/*public interface */
/* include/lib/c */
#include "lib.h"


unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
  unsigned long result = 0,value;

  if (*cp == '0') {
    cp++;
    if ((*cp == 'x') && isxdigit(cp[1])) {
      base = 16;
      cp++;
    }
    if (!base) {
      base = 8;
    }
  }
  if (!base) {
    base = 10;
  }
  while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
                                                             ? toupper(*cp) : *cp)-'A'+10) < base) {
    result = result*base + value;
    cp++;
  }
  if (endp)
    *endp = (char *)cp;

  return result;
}
