##-----------------------------------------------------------------------------
##
## Root CMake file.
##
##-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)

project(DH-acc)

include(CheckCXXCompilerFlag)


# Simplify flag checking and adding.
function(TRY_CXX_COMPILER_FLAG flag name)
	CHECK_CXX_COMPILER_FLAG(${flag} ${name})

	if(${name})
		add_definitions(${flag})
	endif()
endfunction()


# Warnings.
TRY_CXX_COMPILER_FLAG(-Wall    FLAG_CXX_Wall   )
TRY_CXX_COMPILER_FLAG(-Wextra  FLAG_CXX_Wextra )
TRY_CXX_COMPILER_FLAG(-Wshadow FLAG_CXX_Wshadow)

# Features.
TRY_CXX_COMPILER_FLAG(-std=c++11 FLAG_CXX_CXX11)


add_subdirectory(src)

# EOF

