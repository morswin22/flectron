SET(_glm_HEADER_SEARCH_DIRS
"/usr/include"
"/usr/local/include"
"${CMAKE_SOURCE_DIR}/includes"
"C:/Program Files (x86)/glm" )
# check environment variable
SET(_glm_ENV_ROOT_DIR "$ENV{GLM_ROOT_DIR}")
IF(NOT GLM_ROOT_DIR AND _glm_ENV_ROOT_DIR)
	SET(GLM_ROOT_DIR "${_glm_ENV_ROOT_DIR}")
ENDIF(NOT GLM_ROOT_DIR AND _glm_ENV_ROOT_DIR)
# put user specified location at beginning of search
IF(GLM_ROOT_DIR)
	SET(_glm_HEADER_SEARCH_DIRS "${GLM_ROOT_DIR}"
	"${GLM_ROOT_DIR}/include"
	${_glm_HEADER_SEARCH_DIRS})
ENDIF(GLM_ROOT_DIR)
# locate header
FIND_PATH(GLM_INCLUDE_DIR "glm/glm.hpp"
PATHS ${_glm_HEADER_SEARCH_DIRS})
find_library(GLM_LIBRARY NAMES glm)
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLM REQUIRED_VARS GLM_INCLUDE_DIR GLM_LIBRARY)
mark_as_advanced(GLM_INCLUDE_DIR GLM_LIBRARY)
