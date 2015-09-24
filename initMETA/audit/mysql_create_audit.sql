CREATE TABLE IF NOT EXISTS audit_log (
audit_id INT(11) NOT NULL AUTO_INCREMENT ,
audit_date TIMESTAMP NULL ,
audit_action VARCHAR(6) NULL ,
audit_user VARCHAR(120) NULL ,
audit_table VARCHAR(60) NULL ,
audit_column VARCHAR(60) NULL ,
audit_key VARCHAR(500) NULL ,
audit_oldvalue VARCHAR(500) NULL ,
audit_newvalue VARCHAR(500) NULL ,
audit_insdeldata TEXT NULL ,
PRIMARY KEY (audit_id) )
 ENGINE = InnoDB CHARSET=utf8;

delimiter $$
CREATE TRIGGER audit_1_update
AFTER UPDATE ON reg_animregcover
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','rowuuid',CONCAT('(qid)',OLD.qid),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.gpsloc <> NEW.gpsloc THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','gpsloc',CONCAT('(qid)',OLD.qid),OLD.gpsloc,NEW.gpsloc);
END IF;
IF OLD.village <> NEW.village THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','village',CONCAT('(qid)',OLD.qid),OLD.village,NEW.village);
END IF;
IF OLD.location <> NEW.location THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','location',CONCAT('(qid)',OLD.qid),OLD.location,NEW.location);
END IF;
IF OLD.originid <> NEW.originid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','originid',CONCAT('(qid)',OLD.qid),OLD.originid,NEW.originid);
END IF;
IF OLD.surveyid <> NEW.surveyid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','surveyid',CONCAT('(qid)',OLD.qid),OLD.surveyid,NEW.surveyid);
END IF;
IF OLD.qid <> NEW.qid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_animregcover','qid',CONCAT('(qid)',OLD.qid),OLD.qid,NEW.qid);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_1_insert
AFTER INSERT ON reg_animregcover
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_animregcover',CONCAT('(qid)',NEW.qid),CONCAT('(gpsloc)',ifnull(hex(cast(NEW.gpsloc as char)),''),',','(location)',ifnull(hex(cast(NEW.location as char)),''),',','(originid)',ifnull(hex(cast(NEW.originid as char)),''),',','(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),''),',','(surveyid)',ifnull(hex(cast(NEW.surveyid as char)),''),',','(village)',ifnull(hex(cast(NEW.village as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_1_delete
AFTER DELETE ON reg_animregcover
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_animregcover',CONCAT('(qid)',OLD.qid),CONCAT('(gpsloc)',ifnull(hex(cast(OLD.gpsloc as char)),''),',','(location)',ifnull(hex(cast(OLD.location as char)),''),',','(originid)',ifnull(hex(cast(OLD.originid as char)),''),',','(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),''),',','(surveyid)',ifnull(hex(cast(OLD.surveyid as char)),''),',','(village)',ifnull(hex(cast(OLD.village as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_2_update
AFTER UPDATE ON reg_lkpbreed
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpbreed','rowuuid',CONCAT('(breed_cod)',OLD.breed_cod),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.breed_des <> NEW.breed_des THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpbreed','breed_des',CONCAT('(breed_cod)',OLD.breed_cod),OLD.breed_des,NEW.breed_des);
END IF;
IF OLD.breed_cod <> NEW.breed_cod THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpbreed','breed_cod',CONCAT('(breed_cod)',OLD.breed_cod),OLD.breed_cod,NEW.breed_cod);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_2_insert
AFTER INSERT ON reg_lkpbreed
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_lkpbreed',CONCAT('(breed_cod)',NEW.breed_cod),CONCAT('(breed_des)',ifnull(hex(cast(NEW.breed_des as char)),''),',','(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_2_delete
AFTER DELETE ON reg_lkpbreed
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_lkpbreed',CONCAT('(breed_cod)',OLD.breed_cod),CONCAT('(breed_des)',ifnull(hex(cast(OLD.breed_des as char)),''),',','(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_3_update
AFTER UPDATE ON reg_lkpcomposition
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpcomposition','rowuuid',CONCAT('(composition_cod)',OLD.composition_cod),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.composition_des <> NEW.composition_des THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpcomposition','composition_des',CONCAT('(composition_cod)',OLD.composition_cod),OLD.composition_des,NEW.composition_des);
END IF;
IF OLD.composition_cod <> NEW.composition_cod THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpcomposition','composition_cod',CONCAT('(composition_cod)',OLD.composition_cod),OLD.composition_cod,NEW.composition_cod);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_3_insert
AFTER INSERT ON reg_lkpcomposition
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_lkpcomposition',CONCAT('(composition_cod)',NEW.composition_cod),CONCAT('(composition_des)',ifnull(hex(cast(NEW.composition_des as char)),''),',','(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_3_delete
AFTER DELETE ON reg_lkpcomposition
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_lkpcomposition',CONCAT('(composition_cod)',OLD.composition_cod),CONCAT('(composition_des)',ifnull(hex(cast(OLD.composition_des as char)),''),',','(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_4_update
AFTER UPDATE ON reg_lkpdeformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpdeformities','rowuuid',CONCAT('(deformities_cod)',OLD.deformities_cod),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.deformities_des <> NEW.deformities_des THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpdeformities','deformities_des',CONCAT('(deformities_cod)',OLD.deformities_cod),OLD.deformities_des,NEW.deformities_des);
END IF;
IF OLD.deformities_cod <> NEW.deformities_cod THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpdeformities','deformities_cod',CONCAT('(deformities_cod)',OLD.deformities_cod),OLD.deformities_cod,NEW.deformities_cod);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_4_insert
AFTER INSERT ON reg_lkpdeformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_lkpdeformities',CONCAT('(deformities_cod)',NEW.deformities_cod),CONCAT('(deformities_des)',ifnull(hex(cast(NEW.deformities_des as char)),''),',','(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_4_delete
AFTER DELETE ON reg_lkpdeformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_lkpdeformities',CONCAT('(deformities_cod)',OLD.deformities_cod),CONCAT('(deformities_des)',ifnull(hex(cast(OLD.deformities_des as char)),''),',','(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_5_update
AFTER UPDATE ON reg_lkpsex
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsex','rowuuid',CONCAT('(sex_cod)',OLD.sex_cod),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.sex_des <> NEW.sex_des THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsex','sex_des',CONCAT('(sex_cod)',OLD.sex_cod),OLD.sex_des,NEW.sex_des);
END IF;
IF OLD.sex_cod <> NEW.sex_cod THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsex','sex_cod',CONCAT('(sex_cod)',OLD.sex_cod),OLD.sex_cod,NEW.sex_cod);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_5_insert
AFTER INSERT ON reg_lkpsex
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_lkpsex',CONCAT('(sex_cod)',NEW.sex_cod),CONCAT('(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),''),',','(sex_des)',ifnull(hex(cast(NEW.sex_des as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_5_delete
AFTER DELETE ON reg_lkpsex
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_lkpsex',CONCAT('(sex_cod)',OLD.sex_cod),CONCAT('(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),''),',','(sex_des)',ifnull(hex(cast(OLD.sex_des as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_6_update
AFTER UPDATE ON reg_lkpsiretype
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsiretype','rowuuid',CONCAT('(siretype_cod)',OLD.siretype_cod),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.siretype_des <> NEW.siretype_des THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsiretype','siretype_des',CONCAT('(siretype_cod)',OLD.siretype_cod),OLD.siretype_des,NEW.siretype_des);
END IF;
IF OLD.siretype_cod <> NEW.siretype_cod THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_lkpsiretype','siretype_cod',CONCAT('(siretype_cod)',OLD.siretype_cod),OLD.siretype_cod,NEW.siretype_cod);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_6_insert
AFTER INSERT ON reg_lkpsiretype
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_lkpsiretype',CONCAT('(siretype_cod)',NEW.siretype_cod),CONCAT('(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),''),',','(siretype_des)',ifnull(hex(cast(NEW.siretype_des as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_6_delete
AFTER DELETE ON reg_lkpsiretype
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_lkpsiretype',CONCAT('(siretype_cod)',OLD.siretype_cod),CONCAT('(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),''),',','(siretype_des)',ifnull(hex(cast(OLD.siretype_des as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_7_update
AFTER UPDATE ON reg_rpt_animreg
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','rowuuid',CONCAT('(tagid)',OLD.tagid),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.animalphoto <> NEW.animalphoto THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','animalphoto',CONCAT('(tagid)',OLD.tagid),OLD.animalphoto,NEW.animalphoto);
END IF;
IF OLD.tagphoto <> NEW.tagphoto THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','tagphoto',CONCAT('(tagid)',OLD.tagid),OLD.tagphoto,NEW.tagphoto);
END IF;
IF OLD.damcompoth <> NEW.damcompoth THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','damcompoth',CONCAT('(tagid)',OLD.tagid),OLD.damcompoth,NEW.damcompoth);
END IF;
IF OLD.damcomp <> NEW.damcomp THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','damcomp',CONCAT('(tagid)',OLD.tagid),OLD.damcomp,NEW.damcomp);
END IF;
IF OLD.dambreedoth <> NEW.dambreedoth THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','dambreedoth',CONCAT('(tagid)',OLD.tagid),OLD.dambreedoth,NEW.dambreedoth);
END IF;
IF OLD.dambreed <> NEW.dambreed THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','dambreed',CONCAT('(tagid)',OLD.tagid),OLD.dambreed,NEW.dambreed);
END IF;
IF OLD.dambdate <> NEW.dambdate THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','dambdate',CONCAT('(tagid)',OLD.tagid),OLD.dambdate,NEW.dambdate);
END IF;
IF OLD.dambdknow <> NEW.dambdknow THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','dambdknow',CONCAT('(tagid)',OLD.tagid),OLD.dambdknow,NEW.dambdknow);
END IF;
IF OLD.damid <> NEW.damid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','damid',CONCAT('(tagid)',OLD.tagid),OLD.damid,NEW.damid);
END IF;
IF OLD.sirecompoth <> NEW.sirecompoth THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sirecompoth',CONCAT('(tagid)',OLD.tagid),OLD.sirecompoth,NEW.sirecompoth);
END IF;
IF OLD.sirecomp <> NEW.sirecomp THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sirecomp',CONCAT('(tagid)',OLD.tagid),OLD.sirecomp,NEW.sirecomp);
END IF;
IF OLD.sirebreedoth <> NEW.sirebreedoth THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sirebreedoth',CONCAT('(tagid)',OLD.tagid),OLD.sirebreedoth,NEW.sirebreedoth);
END IF;
IF OLD.sirebreed <> NEW.sirebreed THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sirebreed',CONCAT('(tagid)',OLD.tagid),OLD.sirebreed,NEW.sirebreed);
END IF;
IF OLD.sirecnty <> NEW.sirecnty THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sirecnty',CONCAT('(tagid)',OLD.tagid),OLD.sirecnty,NEW.sirecnty);
END IF;
IF OLD.sireid <> NEW.sireid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sireid',CONCAT('(tagid)',OLD.tagid),OLD.sireid,NEW.sireid);
END IF;
IF OLD.siretype <> NEW.siretype THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','siretype',CONCAT('(tagid)',OLD.tagid),OLD.siretype,NEW.siretype);
END IF;
IF OLD.numparity <> NEW.numparity THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','numparity',CONCAT('(tagid)',OLD.tagid),OLD.numparity,NEW.numparity);
END IF;
IF OLD.ccalvdate <> NEW.ccalvdate THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','ccalvdate',CONCAT('(tagid)',OLD.tagid),OLD.ccalvdate,NEW.ccalvdate);
END IF;
IF OLD.fcalvdate <> NEW.fcalvdate THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','fcalvdate',CONCAT('(tagid)',OLD.tagid),OLD.fcalvdate,NEW.fcalvdate);
END IF;
IF OLD.hascalved <> NEW.hascalved THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','hascalved',CONCAT('(tagid)',OLD.tagid),OLD.hascalved,NEW.hascalved);
END IF;
IF OLD.deformitiesoth <> NEW.deformitiesoth THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','deformitiesoth',CONCAT('(tagid)',OLD.tagid),OLD.deformitiesoth,NEW.deformitiesoth);
END IF;
IF OLD.deformities <> NEW.deformities THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','deformities',CONCAT('(tagid)',OLD.tagid),OLD.deformities,NEW.deformities);
END IF;
IF OLD.description <> NEW.description THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','description',CONCAT('(tagid)',OLD.tagid),OLD.description,NEW.description);
END IF;
IF OLD.composition_other <> NEW.composition_other THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','composition_other',CONCAT('(tagid)',OLD.tagid),OLD.composition_other,NEW.composition_other);
END IF;
IF OLD.composition <> NEW.composition THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','composition',CONCAT('(tagid)',OLD.tagid),OLD.composition,NEW.composition);
END IF;
IF OLD.breed_other <> NEW.breed_other THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','breed_other',CONCAT('(tagid)',OLD.tagid),OLD.breed_other,NEW.breed_other);
END IF;
IF OLD.breed <> NEW.breed THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','breed',CONCAT('(tagid)',OLD.tagid),OLD.breed,NEW.breed);
END IF;
IF OLD.sex <> NEW.sex THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','sex',CONCAT('(tagid)',OLD.tagid),OLD.sex,NEW.sex);
END IF;
IF OLD.birthdate <> NEW.birthdate THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','birthdate',CONCAT('(tagid)',OLD.tagid),OLD.birthdate,NEW.birthdate);
END IF;
IF OLD.birthknown <> NEW.birthknown THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','birthknown',CONCAT('(tagid)',OLD.tagid),OLD.birthknown,NEW.birthknown);
END IF;
IF OLD.name <> NEW.name THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','name',CONCAT('(tagid)',OLD.tagid),OLD.name,NEW.name);
END IF;
IF OLD.qid <> NEW.qid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','qid',CONCAT('(tagid)',OLD.tagid),OLD.qid,NEW.qid);
END IF;
IF OLD.tagid <> NEW.tagid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg','tagid',CONCAT('(tagid)',OLD.tagid),OLD.tagid,NEW.tagid);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_7_insert
AFTER INSERT ON reg_rpt_animreg
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_rpt_animreg',CONCAT('(tagid)',NEW.tagid),CONCAT('(animalphoto)',ifnull(hex(cast(NEW.animalphoto as char)),''),',','(birthdate)',ifnull(hex(cast(NEW.birthdate as char)),''),',','(birthknown)',ifnull(hex(cast(NEW.birthknown as char)),''),',','(breed)',ifnull(hex(cast(NEW.breed as char)),''),',','(breed_other)',ifnull(hex(cast(NEW.breed_other as char)),''),',','(ccalvdate)',ifnull(hex(cast(NEW.ccalvdate as char)),''),',','(composition)',ifnull(hex(cast(NEW.composition as char)),''),',','(composition_other)',ifnull(hex(cast(NEW.composition_other as char)),''),',','(dambdate)',ifnull(hex(cast(NEW.dambdate as char)),''),',','(dambdknow)',ifnull(hex(cast(NEW.dambdknow as char)),''),',','(dambreed)',ifnull(hex(cast(NEW.dambreed as char)),''),',','(dambreedoth)',ifnull(hex(cast(NEW.dambreedoth as char)),''),',','(damcomp)',ifnull(hex(cast(NEW.damcomp as char)),''),',','(damcompoth)',ifnull(hex(cast(NEW.damcompoth as char)),''),',','(damid)',ifnull(hex(cast(NEW.damid as char)),''),',','(deformities)',ifnull(hex(cast(NEW.deformities as char)),''),',','(deformitiesoth)',ifnull(hex(cast(NEW.deformitiesoth as char)),''),',','(description)',ifnull(hex(cast(NEW.description as char)),''),',','(fcalvdate)',ifnull(hex(cast(NEW.fcalvdate as char)),''),',','(hascalved)',ifnull(hex(cast(NEW.hascalved as char)),''),',','(name)',ifnull(hex(cast(NEW.name as char)),''),',','(numparity)',ifnull(hex(cast(NEW.numparity as char)),''),',','(qid)',ifnull(hex(cast(NEW.qid as char)),''),',','(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),''),',','(sex)',ifnull(hex(cast(NEW.sex as char)),''),',','(sirebreed)',ifnull(hex(cast(NEW.sirebreed as char)),''),',','(sirebreedoth)',ifnull(hex(cast(NEW.sirebreedoth as char)),''),',','(sirecnty)',ifnull(hex(cast(NEW.sirecnty as char)),''),',','(sirecomp)',ifnull(hex(cast(NEW.sirecomp as char)),''),',','(sirecompoth)',ifnull(hex(cast(NEW.sirecompoth as char)),''),',','(sireid)',ifnull(hex(cast(NEW.sireid as char)),''),',','(siretype)',ifnull(hex(cast(NEW.siretype as char)),''),',','(tagphoto)',ifnull(hex(cast(NEW.tagphoto as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_7_delete
AFTER DELETE ON reg_rpt_animreg
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_rpt_animreg',CONCAT('(tagid)',OLD.tagid),CONCAT('(animalphoto)',ifnull(hex(cast(OLD.animalphoto as char)),''),',','(birthdate)',ifnull(hex(cast(OLD.birthdate as char)),''),',','(birthknown)',ifnull(hex(cast(OLD.birthknown as char)),''),',','(breed)',ifnull(hex(cast(OLD.breed as char)),''),',','(breed_other)',ifnull(hex(cast(OLD.breed_other as char)),''),',','(ccalvdate)',ifnull(hex(cast(OLD.ccalvdate as char)),''),',','(composition)',ifnull(hex(cast(OLD.composition as char)),''),',','(composition_other)',ifnull(hex(cast(OLD.composition_other as char)),''),',','(dambdate)',ifnull(hex(cast(OLD.dambdate as char)),''),',','(dambdknow)',ifnull(hex(cast(OLD.dambdknow as char)),''),',','(dambreed)',ifnull(hex(cast(OLD.dambreed as char)),''),',','(dambreedoth)',ifnull(hex(cast(OLD.dambreedoth as char)),''),',','(damcomp)',ifnull(hex(cast(OLD.damcomp as char)),''),',','(damcompoth)',ifnull(hex(cast(OLD.damcompoth as char)),''),',','(damid)',ifnull(hex(cast(OLD.damid as char)),''),',','(deformities)',ifnull(hex(cast(OLD.deformities as char)),''),',','(deformitiesoth)',ifnull(hex(cast(OLD.deformitiesoth as char)),''),',','(description)',ifnull(hex(cast(OLD.description as char)),''),',','(fcalvdate)',ifnull(hex(cast(OLD.fcalvdate as char)),''),',','(hascalved)',ifnull(hex(cast(OLD.hascalved as char)),''),',','(name)',ifnull(hex(cast(OLD.name as char)),''),',','(numparity)',ifnull(hex(cast(OLD.numparity as char)),''),',','(qid)',ifnull(hex(cast(OLD.qid as char)),''),',','(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),''),',','(sex)',ifnull(hex(cast(OLD.sex as char)),''),',','(sirebreed)',ifnull(hex(cast(OLD.sirebreed as char)),''),',','(sirebreedoth)',ifnull(hex(cast(OLD.sirebreedoth as char)),''),',','(sirecnty)',ifnull(hex(cast(OLD.sirecnty as char)),''),',','(sirecomp)',ifnull(hex(cast(OLD.sirecomp as char)),''),',','(sirecompoth)',ifnull(hex(cast(OLD.sirecompoth as char)),''),',','(sireid)',ifnull(hex(cast(OLD.sireid as char)),''),',','(siretype)',ifnull(hex(cast(OLD.siretype as char)),''),',','(tagphoto)',ifnull(hex(cast(OLD.tagphoto as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_8_update
AFTER UPDATE ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

IF OLD.rowuuid <> NEW.rowuuid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg_msel_deformities','rowuuid',CONCAT('(deformities)',OLD.deformities,',','(tagid)',OLD.tagid),OLD.rowuuid,NEW.rowuuid);
END IF;
IF OLD.deformities <> NEW.deformities THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg_msel_deformities','deformities',CONCAT('(deformities)',OLD.deformities,',','(tagid)',OLD.tagid),OLD.deformities,NEW.deformities);
END IF;
IF OLD.tagid <> NEW.tagid THEN INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) VALUES (ts,'UPDATE',us,'reg_rpt_animreg_msel_deformities','tagid',CONCAT('(deformities)',OLD.deformities,',','(tagid)',OLD.tagid),OLD.tagid,NEW.tagid);
END IF;

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_8_insert
AFTER INSERT ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'INSERT',us,'reg_rpt_animreg_msel_deformities',CONCAT('(deformities)',NEW.deformities,',','(tagid)',NEW.tagid),CONCAT('(rowuuid)',ifnull(hex(cast(NEW.rowuuid as char)),'')));

END$$
DELIMITER ;

delimiter $$
CREATE TRIGGER audit_8_delete
AFTER DELETE ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN
DECLARE ts TIMESTAMP DEFAULT CURRENT_TIMESTAMP();
DECLARE us VARCHAR(120) DEFAULT USER();

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) VALUES (ts,'DELETE',us,'reg_rpt_animreg_msel_deformities',CONCAT('(deformities)',OLD.deformities,',','(tagid)',OLD.tagid),CONCAT('(rowuuid)',ifnull(hex(cast(OLD.rowuuid as char)),'')));

END$$
DELIMITER ;

