DROP DATABASE IF EXISTS `pet_book`;
CREATE DATABASE `pet_book`; 
USE `pet_book`;

CREATE TABLE `pets` (
  `pet_id` int(11) NOT NULL AUTO_INCREMENT,
  `adopter_id` int(11),
  `name` varchar(50) NOT NULL,
  `age_months` int NOT NULL,
  `status` varchar(50) NOT NULL,
  PRIMARY KEY (`pet_id`)
);

ALTER TABLE `pets` AUTO_INCREMENT=1;

# inserting new entries
INSERT INTO `pets` VALUES (1,NULL,'Dummy',0,'in_shelter');
INSERT INTO `pets` VALUES (2,2,'Semi',20,'RIP');
INSERT INTO `pets` VALUES (3,2,'Fredo',40,'RIP');
INSERT INTO `pets` VALUES (4,2,'Tira',20,'adopted');
INSERT INTO `pets` VALUES (5,2,'Misu',20,'adopted');
INSERT INTO `pets` VALUES (6,1,'Watson',20,'adopted');
INSERT INTO `pets` VALUES (7,1,'Sherlock',43,'adopted');
INSERT INTO `pets` VALUES (8,4,'Luna',50,'adopted');
INSERT INTO `pets` VALUES (9,2,'Sol',46,'RIP');
INSERT INTO `pets` VALUES (10,4,'Boltzmann',30,'RIP');
INSERT INTO `pets` VALUES (11,NULL,'Froike',62,'in_shelter');
INSERT INTO `pets` VALUES (12,NULL,'Yochke',62,'RIP');
INSERT INTO `pets` VALUES (13,3,'Chiquito',84,'adopted');
INSERT INTO `pets` VALUES (14,NULL,'Hedwig',28,'in_shelter');
INSERT INTO `pets` VALUES (15,NULL,'Ghost',17,'in_shelter');
INSERT INTO `pets` VALUES (16,NULL,'Shadowfax',200,'in_shelter');
INSERT INTO `pets` VALUES (17,2,'Coffee',2,'adopted');

# fixing autoincrement id column
SET @m = (SELECT MAX(pet_id) + 1 FROM pets); 
SET @s = CONCAT('ALTER TABLE pets AUTO_INCREMENT=', @m);
PREPARE stmt1 FROM @s;
EXECUTE stmt1;
DEALLOCATE PREPARE stmt1;

#autoincrement test (should receive pet_id = 18)
INSERT INTO `pets` (`adopter_id`, `name`, `age_months`, `status`) VALUES (2,'Ccino',2,'adopted')

CREATE TABLE `adopters` (
  `adopter_id` int(11) NOT NULL,
  `name` varchar(50) NOT NULL,
  `phone` int(11) ,
  `email` varchar(50),
  PRIMARY KEY (`adopter_id`)
);

# inserting new entries
INSERT INTO `adopters` VALUES (1,'Lihi',0544444444,'lihi@gmail.com');
INSERT INTO `adopters` VALUES (2,'Tuval',0543434343,'tuval@gmail.com');
INSERT INTO `adopters` VALUES (3,'Snir ',0566666666,'snir@gmail.com');
INSERT INTO `adopters` VALUES (4,'Sonya',039000000,'sonya@gmail.com');
INSERT INTO `adopters` VALUES (5,'Tomer ',031415926,'tomer@gmail.com');

