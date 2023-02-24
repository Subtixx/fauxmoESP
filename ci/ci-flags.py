Import("env")

env.Append(CXXFLAGS="-Wall")
env.Append(CXXFLAGS="-Wextra")
env.Append(CXXFLAGS="-Wno-register")
env.Append(CXXFLAGS="-Wno-unused-function")
env.Append(CXXFLAGS="-std=gnu++17")
env.Append(CFLAGS="-std=gnu17")