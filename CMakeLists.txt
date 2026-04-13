# CMakeLists.txt — SwiftTrack Backend Build Configuration
cmake_minimum_required(VERSION 3.20)
project(SwiftTrack VERSION 1.0.0 LANGUAGES C CXX)

# C++17 standard required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# ============================================================
# Compiler flags
# ============================================================
if(MSVC)
    set(CMAKE_CXX_FLAGS_DEBUG "/Od /Zi /DEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "/O2 /DNDEBUG")
else()
    set(CMAKE_CXX_FLAGS_DEBUG   "-g -O0 -Wall -Wextra -DDEBUG")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

# ============================================================
# Find system packages
# ============================================================

# SQLite3 - Try multiple methods
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
    pkg_check_modules(SQLite3 sqlite3)
endif()

if(NOT SQLite3_FOUND)
    find_package(SQLite3 QUIET)
endif()

if(NOT SQLite3_FOUND)
    # Manual path for Windows or fallback
    if(WIN32)
        set(SQLITE3_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3" CACHE PATH "SQLite3 include directory")
        if(EXISTS "${SQLITE3_INCLUDE_DIR}/sqlite3.h")
            set(SQLite3_FOUND TRUE)
            set(SQLite3_INCLUDE_DIRS "${SQLITE3_INCLUDE_DIR}")
            set(SQLite3_LIBRARIES "sqlite3")
            message(STATUS "SQLite3 found (manual): ${SQLITE3_INCLUDE_DIR}")
        endif()
    else()
        # On Linux/Unix, try common paths
        find_path(SQLITE3_INCLUDE_DIR sqlite3.h
            PATHS
            /usr/include
            /usr/local/include
            /usr/include/sqlite
        )
        find_library(SQLITE3_LIBRARY sqlite3
            PATHS
            /usr/lib
            /usr/local/lib
            /usr/lib/x86_64-linux-gnu
        )
        if(SQLITE3_INCLUDE_DIR AND SQLITE3_LIBRARY)
            set(SQLite3_FOUND TRUE)
            set(SQLite3_INCLUDE_DIRS "${SQLITE3_INCLUDE_DIR}")
            set(SQLite3_LIBRARIES "${SQLITE3_LIBRARY}")
            message(STATUS "SQLite3 found (system): ${SQLITE3_INCLUDE_DIR}")
        endif()
    endif()
endif()

if(NOT SQLite3_FOUND)
    # Fallback to bundled SQLite amalgamation source.
    set(SQLITE3_BUNDLED_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3/sqlite-amalgamation-3470000")
    if(EXISTS "${SQLITE3_BUNDLED_DIR}/sqlite3.h" AND EXISTS "${SQLITE3_BUNDLED_DIR}/sqlite3.c")
        set(SQLite3_FOUND TRUE)
        set(SQLite3_INCLUDE_DIRS "${SQLITE3_BUNDLED_DIR}")
        set(SQLite3_LIBRARIES "")
        set(SQLite3_BUNDLED_SOURCE "${SQLITE3_BUNDLED_DIR}/sqlite3.c")
        message(STATUS "SQLite3 found (bundled amalgamation): ${SQLITE3_BUNDLED_DIR}")
    endif()
endif()

if(SQLite3_FOUND)
    message(STATUS "SQLite3 found: ${SQLite3_INCLUDE_DIRS}")
    message(STATUS "SQLite3 library: ${SQLite3_LIBRARIES}")
else()
    message(FATAL_ERROR "SQLite3 not found")
endif()

# OpenSSL (required by JWT implementation) - Optional for now, will use simplified JWT
find_package(OpenSSL QUIET)
if(NOT OPENSSL_FOUND)
    message(WARNING "OpenSSL not found. JWT will use simplified implementation.")
    set(OPENSSL_FOUND FALSE)
else()
    message(STATUS "OpenSSL found: ${OPENSSL_INCLUDE_DIR}")
endif()

# ============================================================
# Source files
# ============================================================
set(SOURCES
    src/main.cpp
    
    # Database
    src/database/DatabaseManager.cpp
    src/database/UserRepository.cpp
    src/database/ShipmentRepository.cpp
    src/database/TrackingRepository.cpp
    src/database/HubRepository.cpp
    src/database/NotificationRepository.cpp
    
    # Services
    src/services/AuthService.cpp
    src/services/ShipmentService.cpp
    src/services/TrackingService.cpp
    src/services/RouteEngine.cpp
    src/services/AgentService.cpp
    src/services/AnalyticsService.cpp
    src/services/NotificationService.cpp
    
    # Controllers
    src/controllers/AuthController.cpp
    src/controllers/ShipmentController.cpp
    src/controllers/TrackingController.cpp
    src/controllers/AgentController.cpp
    src/controllers/AdminController.cpp
    src/controllers/AnalyticsController.cpp
    
    # Middleware
    src/middleware/AuthMiddleware.cpp
    src/middleware/CorsMiddleware.cpp
    src/middleware/LoggerMiddleware.cpp
    
    # Utils
    src/utils/JwtUtils.cpp
    src/utils/PasswordUtils.cpp
    src/utils/TrackingNumberGenerator.cpp
    src/utils/PriceCalculator.cpp
    src/utils/DateTimeUtils.cpp
    src/utils/ValidationUtils.cpp
    src/utils/JsonUtils.cpp
)

list(APPEND SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/third_party/sqlite3/sqlite-amalgamation-3470000/sqlite3.c)

# ============================================================
# Main executable
# ============================================================
add_executable(swifttrack ${SOURCES})

# ============================================================
# Include directories
# ============================================================
target_include_directories(swifttrack PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/crow/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/nlohmann
    ${SQLite3_INCLUDE_DIRS}
)

if(OPENSSL_FOUND)
    target_include_directories(swifttrack PRIVATE ${OPENSSL_INCLUDE_DIR})
endif()

# ============================================================
# Link libraries
# ============================================================
target_link_libraries(swifttrack PRIVATE
    ${SQLite3_LIBRARIES}
)

# Only link OpenSSL if found
if(OPENSSL_FOUND)
    target_link_libraries(swifttrack PRIVATE OpenSSL::SSL OpenSSL::Crypto)
    target_compile_definitions(swifttrack PRIVATE USE_OPENSSL)
endif()

# Platform-specific linking
if(UNIX AND NOT APPLE)
    target_link_libraries(swifttrack PRIVATE pthread)
endif()

# Windows-specific
if(WIN32)
    target_link_libraries(swifttrack PRIVATE ws2_32 wsock32)
endif()

# ============================================================
# Copy SQL schema to build directory
# ============================================================
configure_file(
    ${CMAKE_SOURCE_DIR}/sql/schema.sql
    ${CMAKE_BINARY_DIR}/schema.sql
    COPYONLY
)

# ============================================================
# Installation
# ============================================================
install(TARGETS swifttrack DESTINATION bin)

message(STATUS "SwiftTrack build configured successfully!")
message(STATUS "Run: cmake -B build && cmake --build build")
message(STATUS "Execute: ./build/swifttrack (Linux/Mac) or build\\swifttrack.exe (Windows)")
