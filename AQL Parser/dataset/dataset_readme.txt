perloc数据是很简单的, 可以用来简单的测试

关于revenue的数据，在aql中创建的其中两个view：一个RevenueAndDivision，另一个RevenueAndDivision2。
实际上应该合成一个view的，但是因为本子集的功能有限，针对不同的pattern但同一种提取信息，只好分开创建view并提取。

(1) StarWars.aql

aql作用：提取《Star Wars: The Force Awakens》电影在圣诞节期间，基于时间和地点情况下的票房收益情况。

数据集内容大意：2015年《Star Wars: The Force Awakens》电影创下圣诞节最高票房纪录。

'Star Wars' Ruling Christmas Weekend Box Office
http://variety.com/2015/film/news/star-wars-the-force-awakens-box-officer-1201667889/

Weekend Box Office
http://www.boxofficeguru.com/weekend.htm

(2) Cavaliers.aql

aql作用：提取NBA克利夫兰骑士队在2015-12-29对战菲尼克斯太阳队的表现情况（比分，评价），以及欧文和詹姆斯的表现情况（得分，评价等）。

数据集内容大意：骑士队和太阳队的比赛中，欧文自受伤复出以来表现最佳的一次，并在最后20多秒大胆投出三分并命中，压制了太阳队反超势头。

Irving's late shooting leads Cavaliers past Suns 101-97
http://espn.go.com/blog/cleveland-cavaliers/post/_/id/1735/big-night-for-kyrie-irving-masks-mediocre-performance-from-the-cavs

Big night for Kyrie Irving masks mediocre performance from the Cavs
http://espn.go.com/blog/cleveland-cavaliers/post/_/id/1735/big-night-for-kyrie-irving-masks-mediocre-performance-from-the-cavs