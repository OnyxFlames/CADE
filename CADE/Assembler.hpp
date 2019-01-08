#pragma once

enum TokenType
{
	Unknown,
	Keyword,
	RegisterLiteral,
	HexLiteral,
};

class Assembler
{
public:
	Assembler();
	~Assembler();
};

