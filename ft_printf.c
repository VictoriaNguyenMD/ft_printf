#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

/*-----------------------FORMAT PROTOTYPE-----------------------*/
/* %[flags][width][.precision][length]specifier */

/*--------------------ALL FORMAT INFORMATION--------------------*/

typedef struct s_formats
{
	int			flags; //done
	int			width; //done
	int			precision; //done
	int			length;
  int 		specifier; //done
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
	d, i, u, o, x, X, f, c, s, p, perc, spec_enum_size
}						t_spec;

typedef struct	s_spec_char_to_int
{
	char	type;
	int		spec_value;
}				t_spec_char_to_int;

t_spec_char_to_int spec_table[spec_enum_size] = 
{
	{'d', d},
	{'i', i},
	{'u', u},
	{'o', o},
	{'x', x},
	{'X', X},
	{'f', f},
	{'c', c},
	{'s', s},
	{'p', p},
  {'%', perc}
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
*/
typedef enum			e_lengths
{
	hh, h, l, ll, L, lengths_enum_size
}						t_lengths;


typedef struct	s_lengths_str_to_int
{
	char	*type;
	int   lengths_value;
}				s_lengths_str_to_int;


s_lengths_str_to_int		lengths_table[lengths_enum_size] =
{
	{"hh", hh},
  {"h", h},
  {"l", l},
  {"ll", ll},
  {"L", L}
};

/*-----------------HELPER FUNCTIONS INFORMATION-----------------*/

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t count;

	count = 0;
	while (*s1 != '\0' && *s2 != '\0' && *s1 == *s2)
	{
		s1++;
		s2++;
	}
	return ((unsigned char)*s1 - (unsigned char)*s2);
}

/*------------------IS CERTAIN TYPE INFORMATION------------------*/

/*
** Algorithm
** 1. Iterate through the spec_table to determine if the character
**    matches a specifier in spec_table. If it does, assign the 
**    associated value to the overall formats->specifier
*/
int is_specifier(char c, t_formats *formats)
{
  int i = 0;
  while (i < spec_enum_size)
  {
    if (spec_table[i].type == c)
    {
      formats->specifier = spec_table[i].spec_value;
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
int ft_isflag(char c, t_formats *formats)
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
** 1. Iterate through the lenghts_table to determine if the string
**    matches a length type in lenghts_table. If it does, assign the 
**    associated value to the overall formats->lengths
*/
int ft_islength(char *c, t_formats *formats)
{
  int i = 0;
  while (i < lengths_enum_size)
  {
    if (ft_strcmp(lengths_table[i].type, c) == 0)
    {
      formats->length = lengths_table[i].lengths_value;
      return 1;
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
** 1. Check Flags //Done
** 2. Width //Done
** 3. Precision
** 4. Length
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
void check_flags(char *str, t_formats *formats, int *i)
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
void check_width(char *str, t_formats *formats, int *i)
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
void check_precision(char *str, t_formats *formats, int *i)
{
  if (str[*i] != '.')
  {
    formats->precision = PREC_NA;
    return;
  }
  (*i)++;
  formats->precision = ft_atoi(&str[*i]);
  while(str[*i] && ft_isdigit(str[*i]))
    (*i)++;
}

void check_length(char *str, t_formats *formats, int *i)
{
  formats->length = 0;
  while(str[*i] && ft_islength(&str[*i], formats)) 
    (*i)++;
}

int main(void) {
  t_formats formats;
  int i = 0;
  t_flags flags;
  char *str = "+19.42hd";
  check_flags(str, &formats, &i);
  check_width(str, &formats, &i);
  check_precision(str, &formats, &i);
  check_length(str, &formats, &i);
  printf("%d\n", formats.length);
  // printf("%d\n", is_specifier('s', &formats));
  // printf("%d", formats.specifier);

  return 0;
}
