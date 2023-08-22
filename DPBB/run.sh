g++ -std=c++11 -O3 -w MDP-bb.cpp -o MDP-bb -DNO_PROGRESS_BAR -DNDEBUG

date >> log/run_log.txt


echo "============== email-Eu-core ================" >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=2_L=2.txt 2 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=2_L=3.txt 2 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=2.txt 3 2 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=3.txt 3 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=3_L=4.txt 3 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=3.txt 4 3 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=4.txt 4 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=4_L=5.txt 4 5 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=5_L=4.txt 5 4 >> log/run_log.txt
timeout 86400s ./MDP-bb ../reduced_graph/email-Eu-core_K=5_L=5.txt 5 5 >> log/run_log.txt


date >> log/run_log.txt
echo "ok" >> log/run_log.txt
echo "" >> log/run_log.txt