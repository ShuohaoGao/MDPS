g++ -std=c++11 -O3 -w MDP-pre.cpp -o MDP-pre

date >> log/pre_log.txt


echo "============== email-Eu-core ================" >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 2 2 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 2 3 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 3 2 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 3 3 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 3 4 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 4 3 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 4 4 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 4 5 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 5 4 >> log/pre_log.txt
./MDP-pre ../data/email-Eu-core.txt 5 5 >> log/pre_log.txt


date >> log/pre_log.txt
echo "ok" >> log/pre_log.txt
echo "" >> log/pre_log.txt