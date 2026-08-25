require_relative '../r4cpp/lib/r4cpp'

pro = R4Cpp::Project.new(name: 'c8cpp')
pro.compiler.flags.std = 26
pro.compiler.flags.cflags.append('-freflection')
# pro.compiler.flags.lflags.append('-Wl,-rpath,/usr/local/gcc-git/lib64')

pro.library(name: 'c8cpp-core') do |lib|
  lib.source(pro.glob('src/c8cpp-core/**/*.cpp'))
  lib.generated_file(pro.glob('src/c8cpp-core/**/*.erb'))
  lib.flags.include_dirs.append('src')
end

pro.library(name: 'c8cpp-network') do |lib|
  lib.source(pro.glob('src/c8cpp-network/**/*.cpp'))
  lib.generated_file(pro.glob('src/c8cpp-network/**/*.erb'))
  lib.depend('c8cpp-core')
  lib.flags.include_dirs.append('src')
end

pro.library(name: 'c8cpp-cipher') do |lib|
  lib.source(pro.glob('src/c8cpp-cipher/**/*.cpp'))
  lib.generated_file(pro.glob('src/c8cpp-cipher/**/*.erb'))
  lib.depend('c8cpp-network')
  lib.depend('c8cpp-core')
  lib.flags.pkg_config('openssl')
  lib.flags.include_dirs.append('src')
end

pro.library(name: 'c8cpp-db') do |lib|
  lib.source(pro.glob('src/c8cpp-db/**/*.cpp'))
  lib.generated_file(pro.glob('src/c8cpp-db/**/*.erb'))
  lib.depend('c8cpp-core')
  lib.flags.pkg_config('sqlite3')
  lib.flags.include_dirs.append('src')
end

pro.test(name: 'test-c8cpp-core') do |exe|
  exe.source(pro.glob('test/c8cpp-core/**/*.cpp'))
  exe.generated_file(pro.glob('test/c8cpp-core/**/*.erb'))
  exe.depend('c8cpp-core')
  exe.flags.libs.append('gmock_main', 'gtest', 'gmock', 'pthread')
end

pro.test(name: 'test-c8cpp-network') do |exe|
  exe.source(pro.glob('test/c8cpp-network/**/*.cpp'))
  exe.generated_file(pro.glob('test/c8cpp-network/**/*.erb'))
  exe.depend('c8cpp-network')
  exe.flags.libs.append('gmock_main', 'gtest', 'gmock', 'pthread')
end

pro.test(name: 'test-c8cpp-cipher') do |exe|
  exe.source(pro.glob('test/c8cpp-cipher/**/*.cpp'))
  exe.generated_file(pro.glob('test/c8cpp-cipher/**/*.erb'))
  exe.depend('c8cpp-cipher')
  exe.flags.libs.append('gmock_main', 'gtest', 'gmock', 'pthread')
end

pro.test(name: 'test-c8cpp-db') do |exe|
  exe.source(pro.glob('test/c8cpp-db/**/*.cpp'))
  exe.generated_file(pro.glob('test/c8cpp-db/**/*.erb'))
  exe.depend('c8cpp-db')
  exe.flags.include_dirs.append('test')
  exe.flags.libs.append('gmock_main', 'gtest', 'gmock', 'pthread')
end

pro.define_tasks
