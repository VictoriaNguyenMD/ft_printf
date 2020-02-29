#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <stdint.h>
#include <limits.h>
/*1*/
/*2*/
/*3*/
/*4*/
/*5*/
/*6*/
/*-----------------------FORMAT PROTOTYPE-----------------------*/
/* %[flags][width][.precision][length]specifier */

/*--------------------ALL FORMAT INFORMATION--------------------*/

typedef struct s_formats
{
	int			flags;
	int			width;
	int			precision;
	int			length;
  int 		specifier;
  int     base;
}				t_formats;

/*------------------ENUM SPECIFIER INFORMATION------------------*/
/*
** Algorithm
** 1. Defined an enum storing the possible specifiers for printf.
**    The enum ends with "spec_enum_size" to determine the number
**    of enums defines since counting begins at 0.
** 2. Defined a struct "s_spec_char_to_int" to store the type
**    and corresponding int value determined by the enum variable
** 3. Defined an array "spec_table" with size ammount according
**    to the number of enum variables. At each array index, there 
**    will be a "s_spec_char_to_int" object that stores a char
**    to later match with the printf string and a corresponding
**    enum value.
** 
** Comments
** 1. The reason why we are using enum is to make the code more 
**    readable; however, you do not have to use enum.
** 2. Alternatively, we could use an array to just store the
**    specifiers as chars.
*/

typedef enum			e_spec
{
	d, i, u, o, x, X, c, s, f, p, perc, spec_enum_size
}						t_spec;

typedef struct	s_spec_char_to_int
{
	char	type;
	char	spec_value;
  int   size;
}				t_spec_char_to_int;

t_spec_char_to_int spec_table[spec_enum_size] = 
{
	{'d', d, 32},
	{'i', i, 32},
	{'u', u, 32},
	{'o', o, 32},
	{'x', x, 32},
	{'X', X, 32},
	{'c', c, 32}, //promoted to int
	{'s', s, 64}, //to print out, va_arg takes in int64_t.
  {'f', f, 64}, //promoted to double
	{'p', p, 64}, //to print out, va_arg takes in int64_t, then print_hex --> int64_t
  {'%', perc, 8} //****FIGURE OUT WHAT TO DO WITH THIS LATER****
};

/*------------------ENUM FLAGS INFORMATION------------------*/
/*
** Algorithm
** 1. Defined an enum to store the possible flags. We are using
**    binary because multiple flags can be valid. We need to
**    check each flag for validity and check the flags that
**    are present.
** 2. Defined a struct "s_flags_char_to_int" to store the flag
**    as a char and its corresponding enum value.
** 3. Defined an array "flags_table" to assign the char to a 
**    specific numerical value according to the enum
**
** Comments
** 1. Binary is used rather than decimal so we can use bitwise
**    operator "or" (|) to turn on/off flags.
*/
typedef enum			e_flags
{
	minus = 0b0001, 
  plus = 0b0010, 
  space = 0b0100,
  hash = 0b1000, 
  zero = 0b10000,
  flags_enum_size = 5
}						t_flags;


typedef struct	s_flags_char_to_int
{
	char	flag;
	int		flag_value;
}				s_flags_char_to_int;


s_flags_char_to_int		flags_table[flags_enum_size] =
{
	{'-', minus},
  {'+', plus},
  {' ', space},
  {'#', hash},
  {'0', zero}
};


/*------------------ENUM LENGTH INFORMATION------------------*/
/*
** Algorithm
** 1. Defined an enum to store the possible length types.
**
** Comments
** 1. It is important to check for double chars first (eg. hh)
**    prior to single chars because we will be using strncmp.
**    As a result, we will check to see if there are "hh"
**    before "h" to ensure
*/
typedef enum			e_lengths
{
	hh, h, ll, l, L, lengths_enum_size
}						t_lengths;


typedef struct	s_lengths_str_to_int
{
	char	*type;
	int   lengths_value;
  int   size;
}				s_lengths_str_to_int;


s_lengths_str_to_int		lengths_table[lengths_enum_size] =
{
	{"hh", hh, 8},
  {"h", h, 16},
  {"ll", ll, 64},
  {"l", l, 64},
  {"L", L, 128} //If Lf, will take in as a long double
};

/*-----------------HELPER FUNCTIONS INFORMATION-----------------*/

int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
  size_t	count;
	int		diff;

	count = 1;
	if (n == 0)
		return (0);
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2 && count++ < n)
	{
		s1++;
		s2++;
	}
	diff = ((unsigned char)*s1 - (unsigned char)*s2);
	return (diff);
}

size_t	ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (*s)
	{
		i++;
		s++;
	}
	return (i);
}

void	ft_putchar(char c)
{
	write(1, &c, 1);
}

void print_unsigned_helper(uintmax_t nbr, t_formats *formats)
{
  if (nbr >= formats->base)
    print_unsigned_helper(nbr / formats->base, formats);
  if (formats->base == 8 || formats->base == 10)
    ft_putchar((char)(nbr % formats->base) + '0');
  else if (formats->base == 16)
  {
    if (nbr % 16 < 10)
      ft_putchar((nbr % 16) + '0');
    else
    {
      if (formats->specifier == x)
        ft_putchar((nbr % 16) - 10 + 'a');
      else if (formats->specifier == X);
        ft_putchar((nbr % 16) - 10 + 'A');
    }
  }
}

void print_signed_helper(intmax_t nbr, t_formats *formats)
{
  if (nbr < 0)
  {
    ft_putchar('-');
    nbr *= -1;
  }
  if (nbr >= formats->base)
    print_signed_helper(nbr / formats->base, formats);
  ft_putchar((nbr % formats->base) + '0');
}


int	ft_nbrlen(intmax_t num)
{
	int count;

	count = 0;
	if (num == 0)
		return (1);
	while (num != 0)
	{
		count++;
		num /= 10;
	}
	return (count);
}

void	*ft_memalloc(size_t size)
{
	unsigned char	*output;
	size_t			i;

	if (!size || size >= SIZE_MAX)
		return (NULL);
	output = (unsigned char*)malloc((size));
	i = 0;
	if (!output)
		return (NULL);
	else
	{
		while (i < (size))
		{
			output[i] = 0;
			i++;
		}
	}
	return (output);
}

char		*ft_itoa(intmax_t n)
{
	char	*str;
	int		nlen;
	long	ln;

	ln = (long)n;
	nlen = ft_nbrlen(n);
	if (n < 0)
	{
		nlen++;
		ln *= -1;
	}
	str = ft_memalloc((nlen + 1) * sizeof(char));
	if (!str)
		return (NULL);
	str[nlen] = '\0';
	while (nlen-- >= 1)
	{
		str[nlen] = (ln % 10) + '0';
		ln /= 10;
	}
	if (n < 0)
		str[0] = '-';
	return (str);
}

size_t	ft_strlen(const char *s)
{
	size_t i;

	i = 0;
	while (*s)
	{
		i++;
		s++;
	}
	return (i);
}

void print_long_double_helper(long double nbr, t_formats *formats)
{
  
}

/*------------------IS CERTAIN TYPE INFORMATION------------------*/

/*
** Algorithm
** 1. Iterate through the spec_table to determine if the character
**    matches a specifier in spec_table. If it does, assign the 
**    associated value to the overall formats->specifier
*/
int ft_isspecifier(char c, t_formats *formats)
{
  int i = 0;
  while (i < spec_enum_size)
  {
    if (spec_table[i].type == c)
    {
      formats->specifier = spec_table[i].size;
      return 1;
    }
    i++;
  }
  return 0;
}

/*
** Function from libft
*/
int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}

/*
** Algorithm
** 1. formats->flags was already initalized to 0.
** 2. Iterate through flags_table. If the flag is present
**    present in the flags_table, then use binary OR 
**    operator (|) to turn on the flag.
** 
** Return
** 1: is a flag
** 0: is not a flag
*/
int ft_isflag(const char c, t_formats *formats)
{
  int i = 0;
  while (i < flags_enum_size)
  {
    if (flags_table[i].flag == c)
    {
      formats->flags |= flags_table[i].flag_value;
      return (1);
    }
    i++;
  }
  return (0);
}

/*
** Algorithm
** 1. Iterate through the lengths_table to determine if the string
**    matches a length type in lengths_table. Because we are looking 
**    for a string rather than a char, we are using ft_strncmp to determine 
**    how many character in the string that we are looking at. If there
**    is a match, then assign the associated value to the overall 
**    formats->lengths.
**
** Comments
** 1. For example, if "ll" is being looked at, we will see if the next 
**    two characters in the string is "ll". It is important to look at
**    double characters before single characters (eg. "l") else you may 
**    find a single character and overlook that it is actually a double
**    character string. 
*/
int ft_islength(const char *str, t_formats *formats)
{
  int i = 0;
  while (i < lengths_enum_size)
  {
    if (ft_strncmp(lengths_table[i].type, str, ft_strlen(lengths_table[i].type)) == 0)
    {
      formats->length = lengths_table[i].lengths_value;
      return ft_strlen(lengths_table[i].type);
    }
    i++;
  }
  return 0;
}

/*------------------CONVERSION------------------*/

int	ft_atoi(const char *str)
{
	int			sign;
	long long	output;

	sign = 1;
	output = 0;
	while ((*str == ' ') || (*str == '\t') || (*str == '\v') ||
	(*str == '\n') || (*str == '\f') || (*str == '\r'))
		str++;
	if (*str == '-')
	{
		sign = -1;
		str++;
	}
	else if (*str == '+')
		str++;
	while ((*str >= '0' && *str <= '9'))
	{
		output = (output * 10) + (*str++ - '0');
	}
	return (output * sign);
}

/*------------------CHECKING FORMATTING------------------*/
/* Things to Check
** 1. Check Flags
** 2. Width
** 3. Precision
** 4. Length
** 5. Base
*/

/*
** Algorithm
** 1. Initalizes formats->flags to 0.
** 2. Iterates through the string until the character is no
**    longer a flag. Checks each char to see if it is a flag.
**
** Comments
** 1. A int * is used because we need to increase the index
**    of the original string index value
*/
void check_flags(const char *str, t_formats *formats, int *i)
{
  formats->flags = 0b00000;
  while(str[*i] && ft_isflag(str[*i], formats)) 
    (*i)++;
}

/*
** Algorithm
** 1. After a flags are checked, the numerical values will
**    be extracted using ft_atoi and assigned to formats->width
** 
** Comments
** 1. A int * is used because we need to increase the index
**    of the original string index value
*/
void check_width(const char *str, t_formats *formats, int *i)
{
  formats->width = ft_atoi(&str[*i]);
  while(str[*i] && ft_isdigit(str[*i]))
    (*i)++;
}

/*
** Algorithm
** 1. After the width is checked, if there is not a '.',
**    then precision is not present in the string. Indicate
**    that precision is not applicable for the string
** 2. If there is a '.', then increment the string and use
**    ft_atoi to extract the number
** 
** Comments
** 1. A int * is used because we need to increase the index
**    of the original string index value
** 2. has precision >= 0
**    no precision < 0
*/
#define PREC_NA -1
void check_precision(const char *str, t_formats *formats, int *i)
{
  formats->precision = PREC_NA;
  if (str[*i] != '.')
    return;
  (*i)++;
  formats->precision = ft_atoi(&str[*i]);
  while(str[*i] && ft_isdigit(str[*i]))
    (*i)++;
}

void check_length(const char *str, t_formats *formats, int *i)
{
  formats->length = 0;
  int increment = ft_islength(&str[*i], formats); 
  *i += increment;
}

void check_specifier(const char *str, t_formats *formats, int *i)
{
  formats->specifier = 0;
  if (ft_isspecifier(str[*i], formats))
    (*i)++;
  check_base(formats);
}

void check_base(t_formats *formats)
{
  formats->base = 10;
  if (formats->flags == o)
    formats->base = 8;
  else if (formats->flags == x || formats->flags == X || formats->flags == p)
    formats->base = 16;
}

/*------------------HANDLING FORMATTING------------------*/

// void handle_flags(char *str, t_formats format)
// {

// }

// void *handle_width(char *str, t_formats format)
  
// }

// void *handle_precision(char *str, t_formats format)
// {
  
// }

// void *handle_length(char *str, t_formats format)
// {
  
// }

/*------------------HANDLING ARGUMENTS------------------*/

// char *format_arg(long double input, t_formats format)
// {
//   char *str = "";

//   return str;
// }

void print_arg(t_formats *formats, va_list *argptr)
{
  if (formats->specifier == d || formats->specifier == i)
    print_signed(formats, argptr);
  else if (formats->specifier == u || formats->specifier == o || formats->specifier == x || formats->specifier == X)
    print_unsigned(formats, argptr);
  else if (formats->specifier == f)
    print_float(formats, argptr);
  else if (formats->specifier == c)
    print_char(formats, argptr);
  else if (formats->specifier == s)
    print_string(formats, argptr);
  else if (formats->specifier == p)
    print_string(formats, argptr);
}

/*-------------------------DISPATCH--------------------------*/
typedef void print_functions(t_formats *formats, va_list *argptr);

print_functions *print_functs[10] = {
  print_signed, //d,i --> Within this, check for length modifiers & hh,h,l,ll
  print_unsigned, //u,o,x,X --> check for length modifiers & hh,h,l,ll
  print_float, //f --> check for length modifiers L
  print_char,
  print_string,
  print_pointer
};

/*-------------------PRINTING FUNCTIONS----------------------*/
void print_signed(t_formats *formats, va_list *argptr)
{
  intmax_t nbr;
  nbr = va_arg(*argptr, intmax_t);
  if (formats->specifier == d || formats->specifier == i)
  {
    if (formats->length == hh)
      nbr = (signed char)nbr;
    else if (formats->length == h)
      nbr = (short int) nbr;
    else if (formats->length == l)
      nbr = (long int) nbr;
    else if (formats->length == ll)
      nbr = (long long int) nbr;
  }
  print_signed_helper(nbr, formats);
}

void print_unsigned(t_formats *formats, va_list *argptr)
{
  uintmax_t nbr;
  nbr = va_arg(*argptr, uintmax_t);
  if (formats->specifier == d || formats->specifier == i)
  {
    if (formats->length == hh)
      nbr = (unsigned char)nbr;
    else if (formats->length == h)
      nbr = (short unsigned int) nbr;
    else if (formats->length == l)
      nbr = (long unsigned int) nbr;
    else if (formats->length == ll)
      nbr = (long long unsigned int) nbr;
  }
  print_unsigned_helper(nbr, formats);
}

void print_float(t_formats *formats, va_list *argptr)
{
  if (formats->length == L)
  {
    long double nbr = va_arg(*argptr, long double);
    print_long_double_helper(nbr, formats);
    return ;
  }
  double nbr;
  nbr = va_arg(*argptr, double);
  nbr = (float)nbr;

  char* characteristic = ft_itoa((intmax_t)nbr);
  int mult = 1;
  int digit = 6;
  while (digit > 0)
  {
    mult = mult * 10;
    digit--;
  }
  char *mantissa = ft_itoa(((intmax_t)(nbr * (float)mult)) % mult);
  write(1, characteristic, ft_strlen(characteristic));
  write(1, ".", 1);
  write(1, mantissa, ft_strlen(mantissa));  
}

void print_char(t_formats *formats, va_list *argptr)
{
  ft_putchar(va_arg(*argptr, char));
}

void print_string(t_formats *formats, va_list *argptr)
{
  char *str;
  str = va_arg(*argptr, char *);
  int i = 0;
  while (str[i])
  {
    ft_putchar(str[i]);
    i++;
  }
}

void print_pointer(t_formats *formats, va_list *argptr)
{
  intmax_t nbr;
  nbr = va_arg(*argptr, intmax_t);
  write (1, "0x", 2);
  print_unsigned_helper(nbr, formats);
}

/*------------------MAIN FT_PRINTF FUNCTION------------------*/

void ft_printf(const char *str, ...)
{
  t_formats formats;
  va_list argptr;
  int i  = 0;

  va_start(argptr, str);
  while (str[i])
  {
    if (str[i] != '%')
    {
      write (1, &str[i], 1);
      i++;     
    } else {
      i++;
      check_flags(str, &formats, &i);
      check_width(str, &formats, &i);
      check_precision(str, &formats, &i);
      check_length(str, &formats, &i);
      check_specifier(str, &formats, &i);
      dispatch_functs(&formats, &argptr);
    }
  }
  va_end(argptr);
}

int main(void) {
  char *str = "Hello %s. This is %s.";
  ft_printf(str, "Joe", "Bob");
  return 0;
}
