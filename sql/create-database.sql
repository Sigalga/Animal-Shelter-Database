DROP DATABASE IF EXISTS `pet_book`;
CREATE DATABASE `pet_book`; 
USE `pet_book`;

CREATE TABLE `pets` (
  `pet_id` int(11) NOT NULL,
  `adopter_id` int(11),
  `name` varchar(50) NOT NULL,
  `age_months` int NOT NULL,
  `status` varchar(50) NOT NULL,
  PRIMARY KEY (`pet_id`)
);

INSERT INTO `pets` VALUES (1,NULL,'Dummy',0,'in shelter');
