#!/bin/sh 
#
#
#-- config --#
ssh_cmd='/usr/bin/ssh'
scp_cmd='/usr/bin/scp'
mkfifo_cmd='/usr/bin/mkfifo'

base_dir=`pwd`
run_dir="${base_dir}/.tmp"
ssh_opts="${base_dir}/ssh.options"
setsid_cmd="${base_dir}/setsid"
host_list_dir="${base_dir}/hostgroups"
control_path="${run_dir}/ssh-%r-%h-%p.sock"

#-- Load Subroutines --#
. sshtool.subr
parse_cmd_args $@

if [ ${_hosts_login} -eq 1 ]; then 
     printf "Logging into...\n" 
     init_session ${host_list}
     ssh_to_hosts

elif [ ${_hosts_login_root_shell} -eq 1 ]; then
     printf "Logging into...\n"
     init_session ${host_list}
     ssh_root_shell

elif [ ${_cpy_to_hosts} -eq 1 ]; then
     printf "Sending files...\n"
     read_src_files
     init_session ${host_list}
     cpy_to_hosts

elif [ ${_exec_hosts} -eq 1 ]; then 
     printf "Run commands...\n" 
     init_session ${host_list}
     exec_on_hosts

elif [ ${_open_conns} -eq 1 ];then 
     printf "Setting up Multiplex connections, and exit...\n" 
     init_session ${host_list}
     printf "Done.\n" 
     verbose_msg "Control path for sessions: ${control_path}" 1  
     exit 1 

elif [ ${_list_hosts} -eq 1 ];then
     printf "\n[${host_list}] :- `grep '^# Description' ${host_list_dir}/${host_list} | head -1 | cut -d: -f2`\n\n"
     strings ${host_list_dir}/${host_list} | grep -v '#' 
     printf "\n"
     exit 1

else 
   usage 
fi 

cleanup
