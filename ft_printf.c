#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>

/*-----------------------FORMAT PROTOTYPE-----------------------*/
/* %[flags][width][.precision][length]specifier */


/*------------------ENUM SPECIFIER INFORMATION------------------*/
/*
** Algorithm
** 1. Defined a struct "s_spec_char_to_int" to store the type
**    and corresponding int value determined by the enum variable
** 2. Defined an array "spec_table" with size ammount according
**    to the number of enum variables. At each array index, there 
**    will be a "s_spec_char_to_int" object that stores a char
**    to later match with the printf string and a corresponding
**    enum value.
** 3. Defined an enum storing the possible specifiers for printf.
**    The enum ends with "spec_enum_size" to determine the number
**    of enums defines since counting begins at 0.
** Expanation
** 1. The reason why we are storing it in an array 
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
** The reason why we are using binary is because there could be 
** multiple flags that are valid. You want to check which ones
** are valid
0
3
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

/*------------------ALL FORMAT INFORMATION------------------*/

typedef struct s_formats
{
	int			flags;
	int			width;
	int			precision;
	int			length;
  int 		specifier;
}				t_formats;


/*------------------IS CERTAIN TYPE INFORMATION------------------*/

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

int	ft_isdigit(int c)
{
	if (c >= '0' && c <= '9')
		return (1);
	else
		return (0);
}


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

//1. Check Flags
//2. Width
//3. Precision
//4. Length


void check_flags(char *str, t_formats *formats, int *i)
{
  formats->flags = 0b00000;
  while(str[*i] && ft_isflag(str[*i], formats)) 
    (*i)++;
}

void check_width(char *str, t_formats *formats, int *i)
{
  formats->width = ft_atoi(&str[*i]);
  while(str[*i] && ft_isdigit(str[*i]))
    (*i)++;
}

void check_precision(char *str, t_formats *formats, int *i)
{
  if (str[*i] == '.')
    (*i)++;
  formats->precision = ft_atoi(&str[*i]);
  while(str[*i] && ft_isdigit(str[*i]))
    (*i)++;
}

void check_length(char *str, t_formats *formats, int *i)
{
  //pass
}

int main(void) {
  char *str = "Hello %+3.42hhd Joe";
  t_formats formats;
  int i = 0;
  t_flags flags;
  check_flags("+-19.42d", &formats, &i);
  // printf("%d", formats.flags);
  // check_width("+19.42d", &formats, &i);
  // check_precision("+19.42d", &formats, &i);
  // printf("%d\n", i);
  // printf("%d", formats.precision);
  // printf("%d\n", is_specifier('s', &formats));
  // printf("%d", formats.specifier);

  return 0;
}
