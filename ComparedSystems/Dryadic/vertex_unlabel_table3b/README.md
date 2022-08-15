# Dryadic

## Stage 1: Run docker image and reproduce the result of table 3b.
    
```shell
docker pull weiyihua/dryadic_test

#Replace 665014af3aca with the real image id. 
docker run -it -u root 665014af3aca /bin/bash

cd home/DryadicBin/vertex_unlabel

#Then run the following commands to do the test. 
```
    
    ./bin/5_12_best ../graphs/Wikivote/snap.txt				// 1.g
    ./bin/5_14_best ../graphs/Wikivote/snap.txt				// 2.g
    ./bin/5_15_best ../graphs/Wikivote/snap.txt				// 3.g
    ./bin/5_16_best ../graphs/Wikivote/snap.txt				// 4.g
    ./bin/5_17_best ../graphs/Wikivote/snap.txt				// 5.g
    ./bin/5_18_best ../graphs/Wikivote/snap.txt				// 6.g
    ./bin/5_19_best ../graphs/Wikivote/snap.txt				// 7.g
    ./bin/5_20_best ../graphs/Wikivote/snap.txt				// 8.g
    ./bin/6_47_best ../graphs/Wikivote/snap.txt				// 9.g
    ./bin/6_28_best ../graphs/Wikivote/snap.txt				// 10.g
    ./bin/6_50_best ../graphs/Wikivote/snap.txt				// 11.g
    ./bin/6_53_best ../graphs/Wikivote/snap.txt				// 12.g
    ./bin/6_62_best ../graphs/Wikivote/snap.txt				// 13.g
    ./bin/6_107_best ../graphs/Wikivote/snap.txt				// 14.g
    ./bin/6_110_best ../graphs/Wikivote/snap.txt				// 15.g
    ./bin/6_111_best ../graphs/Wikivote/snap.txt				// 16.g
    ./bin/7_830_best ../graphs/Wikivote/snap.txt				// 17.g
    ./bin/7_845_best ../graphs/Wikivote/snap.txt				// 18.g
    ./bin/7_846_best ../graphs/Wikivote/snap.txt				// 19.g
    ./bin/7_847_best ../graphs/Wikivote/snap.txt				// 20.g
    ./bin/7_848_best ../graphs/Wikivote/snap.txt				// 21.g
    ./bin/7_850_best ../graphs/Wikivote/snap.txt				// 22.g
    ./bin/7_851_best ../graphs/Wikivote/snap.txt				// 23.g
    ./bin/7_852_best ../graphs/Wikivote/snap.txt				// 24.g
                    
    ./bin/5_12_best ../graphs/Enron/snap.txt				// 1.g
    ./bin/5_14_best ../graphs/Enron/snap.txt				// 2.g
    ./bin/5_15_best ../graphs/Enron/snap.txt				// 3.g
    ./bin/5_16_best ../graphs/Enron/snap.txt				// 4.g
    ./bin/5_17_best ../graphs/Enron/snap.txt				// 5.g
    ./bin/5_18_best ../graphs/Enron/snap.txt				// 6.g
    ./bin/5_19_best ../graphs/Enron/snap.txt				// 7.g
    ./bin/5_20_best ../graphs/Enron/snap.txt				// 8.g
    ./bin/6_47_best ../graphs/Enron/snap.txt				// 9.g
    ./bin/6_28_best ../graphs/Enron/snap.txt				// 10.g
    ./bin/6_50_best ../graphs/Enron/snap.txt				// 11.g
    ./bin/6_53_best ../graphs/Enron/snap.txt				// 12.g
    ./bin/6_62_best ../graphs/Enron/snap.txt				// 13.g
    ./bin/6_107_best ../graphs/Enron/snap.txt				// 14.g
    ./bin/6_110_best ../graphs/Enron/snap.txt				// 15.g
    ./bin/6_111_best ../graphs/Enron/snap.txt				// 16.g
    ./bin/7_830_best ../graphs/Enron/snap.txt				// 17.g
    ./bin/7_845_best ../graphs/Enron/snap.txt				// 18.g
    ./bin/7_846_best ../graphs/Enron/snap.txt				// 19.g
    ./bin/7_847_best ../graphs/Enron/snap.txt				// 20.g
    ./bin/7_848_best ../graphs/Enron/snap.txt				// 21.g
    ./bin/7_850_best ../graphs/Enron/snap.txt				// 22.g
    ./bin/7_851_best ../graphs/Enron/snap.txt				// 23.g
    ./bin/7_852_best ../graphs/Enron/snap.txt				// 24.g
                    
                    
    ./bin/5_12_best ../graphs/MiCo/snap.txt				// 1.g
    ./bin/5_14_best ../graphs/MiCo/snap.txt				// 2.g
    ./bin/5_15_best ../graphs/MiCo/snap.txt				// 3.g
    ./bin/5_16_best ../graphs/MiCo/snap.txt				// 4.g
    ./bin/5_17_best ../graphs/MiCo/snap.txt				// 5.g
    ./bin/5_18_best ../graphs/MiCo/snap.txt				// 6.g
    ./bin/5_19_best ../graphs/MiCo/snap.txt				// 7.g
    ./bin/5_20_best ../graphs/MiCo/snap.txt				// 8.g
    ./bin/6_47_best ../graphs/MiCo/snap.txt				// 9.g
    ./bin/6_28_best ../graphs/MiCo/snap.txt				// 10.g
    ./bin/6_50_best ../graphs/MiCo/snap.txt				// 11.g
    ./bin/6_53_best ../graphs/MiCo/snap.txt				// 12.g
    ./bin/6_62_best ../graphs/MiCo/snap.txt				// 13.g
    ./bin/6_107_best ../graphs/MiCo/snap.txt				// 14.g
    ./bin/6_110_best ../graphs/MiCo/snap.txt				// 15.g
    ./bin/6_111_best ../graphs/MiCo/snap.txt				// 16.g
    ./bin/7_830_best ../graphs/MiCo/snap.txt				// 17.g
    ./bin/7_845_best ../graphs/MiCo/snap.txt				// 18.g
    ./bin/7_846_best ../graphs/MiCo/snap.txt				// 19.g
    ./bin/7_847_best ../graphs/MiCo/snap.txt				// 20.g
    ./bin/7_848_best ../graphs/MiCo/snap.txt				// 21.g
    ./bin/7_850_best ../graphs/MiCo/snap.txt				// 22.g
    ./bin/7_851_best ../graphs/MiCo/snap.txt				// 23.g
    ./bin/7_852_best ../graphs/MiCo/snap.txt				// 24.g
