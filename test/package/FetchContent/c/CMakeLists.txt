cmake_minimum_required(VERSION 3.11)
project(example_spglib LANGUAGES C)

include(FetchContent)

if (DEFINED ENV{PACKIT_SOURCE_URL})
	set(git_repo $ENV{PACKIT_SOURCE_URL})
else ()
	set(git_repo https://github.com/spglib/spglib)
endif ()
if (DEFINED ENV{PACKIT_SOURCE_SHA})
	set(git_tag $ENV{PACKIT_SOURCE_SHA})
elseif (DEFINED ENV{PACKIT_COMMIT_SHA})
	set(git_tag $ENV{PACKIT_COMMIT_SHA})
else ()
	set(git_tag develop)
endif ()

FetchContent_Declare(Spglib
		GIT_REPOSITORY ${git_repo}
		GIT_TAG ${git_tag}
)
FetchContent_MakeAvailable(Spglib)

add_executable(main main.c)
target_link_libraries(main PRIVATE Spglib::symspg)
