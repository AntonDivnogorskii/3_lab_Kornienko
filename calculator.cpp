/*
 	 Calculator.cpp 
   Модуль, отвечающий за вычисление выражений.
   Парсер выражения подаётся в функцию evaluate_expression.
   Все промежуточные вычисления и результаты в формате float.
 
   Особые ситуации:
      В случае возникновения ошибок выбрасываются исключения типа 
    const char *
*/ 

#include "stdafx.h"
#include <cmath>
#include "calculator.h"
#include "name_table.h"

// Чтение первичного - т.е. минимально неделимого элемента выражения
float prim(Parser& parser) {
  parser.get_lexem();

  switch (parser.get_last().type) {
    case LT_Number: 
      {
        float v = parser.get_last().value;
        parser.get_lexem();
        return v; 
      }
    case LT_Identifier:
      {
        std::string name(parser.get_last().name);
        parser.get_lexem();
        return NT.GetVariable(name); 
      }
      break;
    case LT_Delimiter:
      switch(parser.get_last().delimiter) {
    case '-':
      return -prim(parser);
    case '(': 
      {
        float e=evaluate_expression(parser);
        if (parser.get_last().type == LT_Delimiter && parser.get_last().delimiter == ')' ) {
          parser.get_lexem();
          return e;
        } else
          throw "\')\' expected";
      }
    default:
      throw "primary expected";
      }
    default:
      throw "primary expected";
  }
}

// Обрабатывает возведение в степень
float power(Parser& parser) {
    float left = prim(parser);

    if (parser.get_last().type == LT_Delimiter &&
        parser.get_last().delimiter == '^') {
        float right = power(parser);
        return static_cast<float>(pow(left, right));
    }

    return left;
}

// Обрабатывает умножение и деление
float term(Parser& parser) {
    float left = power(parser);

  for (;;) {
    if (parser.get_last().type == LT_Delimiter) {
      switch (parser.get_last().delimiter)
      {
      case '*':
          left *= power(parser);
        break;
      case '/':
        { const float precision=1.0e-5f;
        float d = power(parser);
        if (fabs(d) < precision) {
            throw "Devide by zero";
        } else
            left /= d;
        }
        break;
      default:
        return left;
      }
    } else
      return left;
  }
  return left;
}

// Обрабатывает сложение и вычитание
float plus_minus(Parser& parser) {
  float left = term(parser);

  for (;;) {
    if (parser.get_last().type == LT_Delimiter) {
      switch (parser.get_last().delimiter)
      {
      case '+':
        left+=term(parser);
        break;
      case '-':
        left-=term(parser);
        break;
      default:
        return left;
      }
    } else
      return left;
  }
  return left;
}


// Вычисляет выражение
float evaluate_expression(Parser& parser) {
  float left = plus_minus(parser);

  for (;;) {
    if (parser.get_last().type == LT_Delimiter) {
      switch (parser.get_last().delimiter)
      {
      case '<':
        left = left < plus_minus(parser);
        break;
      case '>':
        left = left > plus_minus(parser);
        break;
      case '=':
        left = left == plus_minus(parser);
        break;
      default:
        return left;
      }
    } else
      return left;
  }
  return left;
}