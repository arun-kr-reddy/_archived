# tools
- [linux](#linux)
- [cmake](#cmake)

## links  <!-- omit from toc -->

## todo  <!-- omit from toc -->
- [git](http://rogerdudler.github.io/git-guide/)
  - [git parable](https://www.youtube.com/watch?v=jm7QsI-nNjk)
- [regex basics](https://www.youtube.com/watch?v=sa-TUpSx1JA)
- gdb
- cmake
- gtest

## linux
- ```sh
  arun@IG3:~$                   # arun@IG3 (user@machine), ~ (/home/), $ normal user (root user #)
  ls                            # -l (list), -a (show all files), -R (recursive)
  mkdir                         # -p (create parent directories)
  file                          # file info
  man                           # command manual page
  cp                            # -r (recursive)
  mv                            # always recursive
  rm                            # -r (recursive)
  cat                           # print file
  >                             # write 1> (stdout), 2> (stderr), 2>&1 (both)
  >>                            # append
  nano                          # basic text editor
  sort                          # -n (numerical sort, default string sort), -uonly (unique entries)
  find <path> -name "<string>"  # search for string filename in path
  grep <string> <file>          # -n (print line numbers), -r (recursive), -i (case insensitive)
  size <exe>                    # memory layout sizes
  tar                           # -c (compress), -x (extract), -v (show files), -f (file)
  ```
- **chaining commands:**
  ```sh
  command1 ; command2   # sequential
  command1 && command2  # if previous command successful
  command1 | command2   # current stdout to next stdin (stderr to terminal)
  ```
- **placeholder:**
  ```sh
  *        # any set of characters
  ?        # single char
  [a-c]    # single char in range
  [a,b,c]  # single char in list
  [^a-c]   # single char not in range, similr for list
  ```
- **file permissions:** 3 bits each for `r`ead, `w`rite & e`x`ecute for user, group & everyone else
  ```sh
  chmod +700 file  # change mode, 700 means only user can r, w or x
  ```
- **dynamic lib versions:**
  ```sh
  libtest.so ⟶ libtest.so.2 ⟶ libtest.so.2.1  # lib ⟶ lib_major_ver ⟶ lib_minor_ver
  ```

## cmake
- doesn't build code, generates files to feed into build system