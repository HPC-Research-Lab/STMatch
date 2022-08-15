# Labeled

## Stage 1: Run docker image and reproduce the result of table 4.
    
```shell
docker pull weiyihua/dryadic_test

#Replace 665014af3aca with the real image id. 
docker run -it -u root 665014af3aca /bin/bash

cd home/DryadicBin/

#Then run the following commands to do the test. 
```

    ./bin/5_12.txt_li_best ../graphs//Wikivote/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//Wikivote/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//Wikivote/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//Wikivote/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//Wikivote/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//Wikivote/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//Wikivote/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//Wikivote/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//Wikivote/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//Wikivote/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//Wikivote/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//Wikivote/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//Wikivote/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//Wikivote/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//Wikivote/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//Wikivote/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//Wikivote/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//Wikivote/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//Wikivote/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//Wikivote/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//Wikivote/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//Wikivote/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//Wikivote/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//Wikivote/snap.txt				// 24.g
                    
    ./bin/5_12.txt_li_best ../graphs//Enron/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//Enron/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//Enron/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//Enron/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//Enron/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//Enron/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//Enron/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//Enron/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//Enron/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//Enron/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//Enron/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//Enron/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//Enron/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//Enron/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//Enron/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//Enron/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//Enron/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//Enron/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//Enron/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//Enron/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//Enron/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//Enron/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//Enron/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//Enron/snap.txt				// 24.g
                    
                    
    ./bin/5_12.txt_li_best ../graphs//Youtube/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//Youtube/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//Youtube/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//Youtube/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//Youtube/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//Youtube/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//Youtube/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//Youtube/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//Youtube/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//Youtube/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//Youtube/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//Youtube/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//Youtube/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//Youtube/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//Youtube/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//Youtube/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//Youtube/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//Youtube/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//Youtube/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//Youtube/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//Youtube/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//Youtube/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//Youtube/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//Youtube/snap.txt				// 24.g
                    
    ./bin/5_12.txt_li_best ../graphs//MiCo/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//MiCo/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//MiCo/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//MiCo/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//MiCo/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//MiCo/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//MiCo/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//MiCo/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//MiCo/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//MiCo/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//MiCo/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//MiCo/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//MiCo/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//MiCo/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//MiCo/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//MiCo/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//MiCo/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//MiCo/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//MiCo/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//MiCo/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//MiCo/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//MiCo/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//MiCo/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//MiCo/snap.txt				// 24.g
                    

    #The following 3 extreme large graphs can't be uploaded to the github. 
    #For testing the following 3 large graphs, 
    #please follow the instructions offered in STMatch to download them 
    #and moved them in the directory /project_path/Dryadic/graphs/ .
    #STMatch and Dryadic have the same input graph format. 

    ./bin/5_12.txt_li_best ../graphs//LiveJournal/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//LiveJournal/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//LiveJournal/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//LiveJournal/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//LiveJournal/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//LiveJournal/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//LiveJournal/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//LiveJournal/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//LiveJournal/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//LiveJournal/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//LiveJournal/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//LiveJournal/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//LiveJournal/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//LiveJournal/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//LiveJournal/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//LiveJournal/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//LiveJournal/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//LiveJournal/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//LiveJournal/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//LiveJournal/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//LiveJournal/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//LiveJournal/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//LiveJournal/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//LiveJournal/snap.txt				// 24.g
                    
    ./bin/5_12.txt_li_best ../graphs//Orkut/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//Orkut/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//Orkut/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//Orkut/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//Orkut/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//Orkut/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//Orkut/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//Orkut/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//Orkut/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//Orkut/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//Orkut/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//Orkut/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//Orkut/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//Orkut/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//Orkut/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//Orkut/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//Orkut/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//Orkut/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//Orkut/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//Orkut/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//Orkut/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//Orkut/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//Orkut/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//Orkut/snap.txt				// 24.g
                    
    ./bin/5_12.txt_li_best ../graphs//FriendSter/snap.txt				// 1.g
    ./bin/5_14.txt_li_best ../graphs//FriendSter/snap.txt				// 2.g
    ./bin/5_15.txt_li_best ../graphs//FriendSter/snap.txt				// 3.g
    ./bin/5_16.txt_li_best ../graphs//FriendSter/snap.txt				// 4.g
    ./bin/5_17.txt_li_best ../graphs//FriendSter/snap.txt				// 5.g
    ./bin/5_18.txt_li_best ../graphs//FriendSter/snap.txt				// 6.g
    ./bin/5_19.txt_li_best ../graphs//FriendSter/snap.txt				// 7.g
    ./bin/5_20.txt_li_best ../graphs//FriendSter/snap.txt				// 8.g
    ./bin/6_47.txt_li_best ../graphs//FriendSter/snap.txt				// 9.g
    ./bin/6_28.txt_li_best ../graphs//FriendSter/snap.txt				// 10.g
    ./bin/6_50.txt_li_best ../graphs//FriendSter/snap.txt				// 11.g
    ./bin/6_53.txt_li_best ../graphs//FriendSter/snap.txt				// 12.g
    ./bin/6_62.txt_li_best ../graphs//FriendSter/snap.txt				// 13.g
    ./bin/6_107.txt_li_best ../graphs//FriendSter/snap.txt				// 14.g
    ./bin/6_110.txt_li_best ../graphs//FriendSter/snap.txt				// 15.g
    ./bin/6_111.txt_li_best ../graphs//FriendSter/snap.txt				// 16.g
    ./bin/7_830.txt_li_best ../graphs//FriendSter/snap.txt				// 17.g
    ./bin/7_845.txt_li_best ../graphs//FriendSter/snap.txt				// 18.g
    ./bin/7_846.txt_li_best ../graphs//FriendSter/snap.txt				// 19.g
    ./bin/7_847.txt_li_best ../graphs//FriendSter/snap.txt				// 20.g
    ./bin/7_848.txt_li_best ../graphs//FriendSter/snap.txt				// 21.g
    ./bin/7_850.txt_li_best ../graphs//FriendSter/snap.txt				// 22.g
    ./bin/7_851.txt_li_best ../graphs//FriendSter/snap.txt				// 23.g
    ./bin/7_852.txt_li_best ../graphs//FriendSter/snap.txt				// 24.g
