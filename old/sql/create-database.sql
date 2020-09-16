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
INSERT INTO `pets` VALUES (1,NULL,'Dummy',0,'in shelter');

# fixing autoincrement id column
SET @m = (SELECT MAX(pet_id) + 1 FROM pets); 
SET @s = CONCAT('ALTER TABLE pets AUTO_INCREMENT=', @m);
PREPARE stmt1 FROM @s;
EXECUTE stmt1;
DEALLOCATE PREPARE stmt1;