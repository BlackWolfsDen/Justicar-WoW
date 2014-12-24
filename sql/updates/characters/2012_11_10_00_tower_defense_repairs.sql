/*
SQLyog Ultimate v9.50 
MySQL - 5.5.16 
*********************************************************************
*/
/*!40101 SET NAMES utf8 */;

UPDATE `custom_td_waves_data` SET `pathId` = 95051 WHERE `pathId` = 99000;
drop table `custom_td_config`;
create table `custom_td_config` (
	`pSpawnX` float ,
	`pSpawnY` float ,
	`pSpawnZ` float ,
	`pSpawnO` float ,
	`minLvl` int (12),
	`awardFled` tinyint (1),
	`disableGM` tinyint (1),
	`startResources` int (12),
	`disableEvent` tinyint (1),
	`buildItemEntry` int (12),
	`quitAfterWave` int (12),
	`disableFileLog` tinyint (1),
	`gobPlatformEntry` int (12)
); 
insert into `custom_td_config` (`pSpawnX`, `pSpawnY`, `pSpawnZ`, `pSpawnO`, `minLvl`, `awardFled`, `disableGM`, `startResources`, `disableEvent`, `buildItemEntry`, `quitAfterWave`, `disableFileLog`, `gobPlatformEntry`) values('15.6391','465.288','-22.9098','4.6877','255','1','0','5000','0','34131','5','0','800501');
