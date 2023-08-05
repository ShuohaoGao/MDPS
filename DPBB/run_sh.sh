echo "run start"
g++ -std=c++11 ./entrance.cpp -o ./entrance.out -O2
# timeout 86400s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=4_L\=5.txt 4 5
# timeout 86400s ./entrance.out ./reduced_graph/WikiTalk_K\=5_L\=5.txt 5 5

timeout 3600s ./entrance.out ./reduced_graph/enron_K\=4_L\=5.txt 4 5
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=5_L\=4.txt 5 4
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=2_L\=2.txt 2 2
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=2_L\=3.txt 2 3
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=3_L\=2.txt 3 2
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=3_L\=3.txt 3 3
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=3_L\=4.txt 3 4
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=4_L\=3.txt 4 3
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=4_L\=4.txt 4 4
timeout 3600s ./entrance.out ./reduced_graph/enron_K\=5_L\=5.txt 5 5
echo "run enron"

timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=4_L\=5.txt 4 5
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=5_L\=4.txt 5 4
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=2_L\=2.txt 2 2
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=2_L\=3.txt 2 3
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=3_L\=2.txt 3 2
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=3_L\=3.txt 3 3
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=3_L\=4.txt 3 4
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=4_L\=3.txt 4 3
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=4_L\=4.txt 4 4
timeout 3600s ./entrance.out ./reduced_graph/imdb-2021_K\=5_L\=5.txt 5 5
echo "run imdb-2021"

# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/sx-stackoverflow_K\=5_L\=5.txt 5 5
# echo "run sx-stackoverflow"

# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/wiki-topcats_K\=5_L\=5.txt 5 5
# echo "run wiki-topcats"

# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/web-NotreDame_K\=5_L\=5.txt 5 5
# echo "run web-NotreDame"

# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/web-BerkStan_K\=5_L\=5.txt 5 5
# echo "run web-BerkStan"

# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/Slashdot0811_K\=5_L\=5.txt 5 5
# echo "run Slashdot0811"

# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/Amazon0601_K\=5_L\=5.txt 5 5
# echo "run Amazon0601"

# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/email-eu-all_K\=5_L\=5.txt 5 5
# echo "run email-eu-all"

# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/email-Eu-core_K\=5_L\=5.txt 5 5
# echo "run email-Eu-core"

# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-pokec-relationships_K\=5_L\=5.txt 5 5
# echo "run soc-pokec-relationships"

# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=4_L\=5.txt 4 5
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=5_L\=4.txt 5 4
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=2_L\=2.txt 2 2
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=2_L\=3.txt 2 3
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=3_L\=2.txt 3 2
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=3_L\=3.txt 3 3
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=3_L\=4.txt 3 4
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=4_L\=3.txt 4 3
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=4_L\=4.txt 4 4
# # timeout 3600s ./entrance.out ./reduced_graph/cit-Patents_K\=5_L\=5.txt 5 5
# # echo "run cit-Patents"

# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-LiveJournal1_K\=5_L\=5.txt 5 5
# echo "run soc-LiveJournal1"

# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/web-Google_K\=5_L\=5.txt 5 5
# echo "run web-Google"

# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-Epinions1_K\=5_L\=5.txt 5 5
# echo "run soc-Epinions1"

# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/soc-sign-Slashdot090221_K\=5_L\=5.txt 5 5
# echo "run soc-sign-Slashdot090221"

# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/Wiki-Vote_K\=5_L\=5.txt 5 5
# echo "run Wiki-Vote"

# # timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella06_K\=5_L\=5.txt 5 5
# echo "run p2p-Gnutella06"

# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=4_L\=4.txt 4 4
# timeout 3600s ./entrance.out ./reduced_graph/p2p-Gnutella31_K\=5_L\=5.txt 5 5
# echo "run p2p-Gnutella31"

# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=4_L\=5.txt 4 5
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=5_L\=4.txt 5 4
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=2_L\=2.txt 2 2
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=2_L\=3.txt 2 3
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=3_L\=2.txt 3 2
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=3_L\=3.txt 3 3
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=3_L\=4.txt 3 4
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=4_L\=3.txt 4 3
# timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=4_L\=4.txt 4 4
# # timeout 3600s ./entrance.out ./reduced_graph/WikiTalk_K\=5_L\=5.txt 5 5
# echo "run WikiTalk"

echo "run ok"