add_executable(DH-acc
	main.cpp

	BinaryTokenZDACS.cpp
	ObjectExpression.cpp
	ObjectExpression/Base.cpp
	ObjectExpression/BinaryAnd.cpp
	ObjectExpression/BinaryAdd.cpp
	ObjectExpression/BinaryDiv.cpp
	ObjectExpression/BinaryIOr.cpp
	ObjectExpression/BinaryMod.cpp
	ObjectExpression/BinaryMul.cpp
	ObjectExpression/BinarySub.cpp
	ObjectExpression/BinaryXOr.cpp
	ObjectExpression/UnaryAdd.cpp
	ObjectExpression/UnarySub.cpp
	ObjectExpression/ValueInt32.cpp
	ObjectExpression/ValueSymbol.cpp
	ObjectToken.cpp
	print_debug.cpp
	SourceException.cpp
	SourceExpressionACS.cpp
	SourceExpressionACS/Base.cpp
	SourceExpressionACS/BinaryAdd.cpp
	SourceBlockC.cpp
	SourcePosition.cpp
	SourceStream.cpp
	SourceTokenASMPLX.cpp
	SourceTokenC.cpp
	SourceTokenizerC.cpp
)



