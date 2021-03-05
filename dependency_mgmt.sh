#!/bin/bash

declare -A deps
deps["boost_program_options"]="sudo apt-get install -y libboost-all-dev && echo 'boost successfully installed.'"
deps["boost_system"]="sudo apt-get install -y libboost-all-dev && echo 'boost successfully installed.'"
deps["yaml-cpp"]="cd /tmp; rm -rf yaml-cpp; git clone --depth 1 --branch yaml-cpp-0.6.3 https://github.com/jbeder/yaml-cpp.git && cd yaml-cpp && mkdir build && cd build && cmake .. && make && sudo make install && echo 'yaml-cpp successfully installed.'"
deps["spdlog"]="cd /tmp; rm -rf spdlog; git clone --depth 1 --branch v1.8.2 https://github.com/gabime/spdlog.git && cd spdlog && mkdir build && cd build && cmake .. && make && sudo make install && echo 'spdlog successfully installed.'"
deps["crypt"]="sudo apt-get install -y libcrypt-dev && echo 'crypt successfully installed.'"
deps["crypto"]="sudo apt-get install -y libssl-dev && echo 'crypto successfully installed.'"
deps["curl"]="sudo apt-get install -y libcurl4-openssl-dev && echo 'libcurl successfully installed.'"
deps["ssl"]="sudo apt-get install -y libssl-dev && echo 'crypto successfully installed.'"

deps_to_install=()

check_if_installed ()
{
  output=$(g++ -l"$1" 2>&1)
  if [[ $output == *"undefined reference to \`main'"* ]];
  then
    echo "$1: Installed"
  else
    echo "$1: Not Installed"
    deps_to_install+=("$1")
  fi
}

for i in "${!deps[@]}"
do
  check_if_installed "$i"
done

printf '=%.0s' {1..50}
printf '\n'

if [[ ${#deps_to_install[@]} != 0 ]];
then
  echo "The following dependencies couldn't be resolved: (" "${deps_to_install[@]}" ")"
  read -p "Do you want to install them? " -n 1 -r
  echo
  if [[ $REPLY =~ ^[Yy]$ ]]
  then
    for i in "${deps_to_install[@]}"
    do
      eval "${deps["$i"]}"
    done
  fi
else
  echo "You're good to go!!!"
fi
