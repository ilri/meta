BEGIN;
CREATE TABLE audit_log (
audit_id INTEGER NOT NULL,
audit_date VARCHAR(120) NULL ,
audit_action VARCHAR(6) NULL ,
audit_user VARCHAR(120) NULL ,
audit_table VARCHAR(60) NULL ,
audit_column VARCHAR(60) NULL ,
audit_key VARCHAR(500) NULL ,
audit_oldvalue VARCHAR(500) NULL ,
audit_newvalue VARCHAR(500) NULL ,
audit_insdeldata TEXT NULL ,
PRIMARY KEY (audit_id) );

CREATE TRIGGER audit_1_update
AFTER UPDATE ON reg_animregcover
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','rowuuid',('(qid)' || OLD.qid),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','gpsloc',('(qid)' || OLD.qid),OLD.gpsloc,NEW.gpsloc);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','village',('(qid)' || OLD.qid),OLD.village,NEW.village);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','location',('(qid)' || OLD.qid),OLD.location,NEW.location);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','originid',('(qid)' || OLD.qid),OLD.originid,NEW.originid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','surveyid',('(qid)' || OLD.qid),OLD.surveyid,NEW.surveyid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_animregcover','qid',('(qid)' || OLD.qid),OLD.qid,NEW.qid);

END;

CREATE TRIGGER audit_1_insert
AFTER INSERT ON reg_animregcover
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_animregcover',('(qid)' || NEW.qid),('(gpsloc)' || hex(NEW.gpsloc) || ',' ||'(location)' || hex(NEW.location) || ',' ||'(originid)' || hex(NEW.originid) || ',' ||'(rowuuid)' || hex(NEW.rowuuid) || ',' ||'(surveyid)' || hex(NEW.surveyid) || ',' ||'(village)' || hex(NEW.village) ));

END;

CREATE TRIGGER audit_1_delete
AFTER DELETE ON reg_animregcover
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_animregcover',('(qid)' || OLD.qid),('(gpsloc)' || hex(OLD.gpsloc) || ',' ||'(location)' || hex(OLD.location) || ',' ||'(originid)' || hex(OLD.originid) || ',' ||'(rowuuid)' || hex(OLD.rowuuid) || ',' ||'(surveyid)' || hex(OLD.surveyid) || ',' ||'(village)' || hex(OLD.village) ));

END;

CREATE TRIGGER audit_2_update
AFTER UPDATE ON reg_lkpbreed
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpbreed','rowuuid',('(breed_cod)' || OLD.breed_cod),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpbreed','breed_des',('(breed_cod)' || OLD.breed_cod),OLD.breed_des,NEW.breed_des);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpbreed','breed_cod',('(breed_cod)' || OLD.breed_cod),OLD.breed_cod,NEW.breed_cod);

END;

CREATE TRIGGER audit_2_insert
AFTER INSERT ON reg_lkpbreed
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_lkpbreed',('(breed_cod)' || NEW.breed_cod),('(breed_des)' || hex(NEW.breed_des) || ',' ||'(rowuuid)' || hex(NEW.rowuuid) ));

END;

CREATE TRIGGER audit_2_delete
AFTER DELETE ON reg_lkpbreed
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_lkpbreed',('(breed_cod)' || OLD.breed_cod),('(breed_des)' || hex(OLD.breed_des) || ',' ||'(rowuuid)' || hex(OLD.rowuuid) ));

END;

CREATE TRIGGER audit_3_update
AFTER UPDATE ON reg_lkpcomposition
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpcomposition','rowuuid',('(composition_cod)' || OLD.composition_cod),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpcomposition','composition_des',('(composition_cod)' || OLD.composition_cod),OLD.composition_des,NEW.composition_des);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpcomposition','composition_cod',('(composition_cod)' || OLD.composition_cod),OLD.composition_cod,NEW.composition_cod);

END;

CREATE TRIGGER audit_3_insert
AFTER INSERT ON reg_lkpcomposition
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_lkpcomposition',('(composition_cod)' || NEW.composition_cod),('(composition_des)' || hex(NEW.composition_des) || ',' ||'(rowuuid)' || hex(NEW.rowuuid) ));

END;

CREATE TRIGGER audit_3_delete
AFTER DELETE ON reg_lkpcomposition
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_lkpcomposition',('(composition_cod)' || OLD.composition_cod),('(composition_des)' || hex(OLD.composition_des) || ',' ||'(rowuuid)' || hex(OLD.rowuuid) ));

END;

CREATE TRIGGER audit_4_update
AFTER UPDATE ON reg_lkpdeformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpdeformities','rowuuid',('(deformities_cod)' || OLD.deformities_cod),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpdeformities','deformities_des',('(deformities_cod)' || OLD.deformities_cod),OLD.deformities_des,NEW.deformities_des);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpdeformities','deformities_cod',('(deformities_cod)' || OLD.deformities_cod),OLD.deformities_cod,NEW.deformities_cod);

END;

CREATE TRIGGER audit_4_insert
AFTER INSERT ON reg_lkpdeformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_lkpdeformities',('(deformities_cod)' || NEW.deformities_cod),('(deformities_des)' || hex(NEW.deformities_des) || ',' ||'(rowuuid)' || hex(NEW.rowuuid) ));

END;

CREATE TRIGGER audit_4_delete
AFTER DELETE ON reg_lkpdeformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_lkpdeformities',('(deformities_cod)' || OLD.deformities_cod),('(deformities_des)' || hex(OLD.deformities_des) || ',' ||'(rowuuid)' || hex(OLD.rowuuid) ));

END;

CREATE TRIGGER audit_5_update
AFTER UPDATE ON reg_lkpsex
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsex','rowuuid',('(sex_cod)' || OLD.sex_cod),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsex','sex_des',('(sex_cod)' || OLD.sex_cod),OLD.sex_des,NEW.sex_des);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsex','sex_cod',('(sex_cod)' || OLD.sex_cod),OLD.sex_cod,NEW.sex_cod);

END;

CREATE TRIGGER audit_5_insert
AFTER INSERT ON reg_lkpsex
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_lkpsex',('(sex_cod)' || NEW.sex_cod),('(rowuuid)' || hex(NEW.rowuuid) || ',' ||'(sex_des)' || hex(NEW.sex_des) ));

END;

CREATE TRIGGER audit_5_delete
AFTER DELETE ON reg_lkpsex
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_lkpsex',('(sex_cod)' || OLD.sex_cod),('(rowuuid)' || hex(OLD.rowuuid) || ',' ||'(sex_des)' || hex(OLD.sex_des) ));

END;

CREATE TRIGGER audit_6_update
AFTER UPDATE ON reg_lkpsiretype
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsiretype','rowuuid',('(siretype_cod)' || OLD.siretype_cod),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsiretype','siretype_des',('(siretype_cod)' || OLD.siretype_cod),OLD.siretype_des,NEW.siretype_des);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_lkpsiretype','siretype_cod',('(siretype_cod)' || OLD.siretype_cod),OLD.siretype_cod,NEW.siretype_cod);

END;

CREATE TRIGGER audit_6_insert
AFTER INSERT ON reg_lkpsiretype
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_lkpsiretype',('(siretype_cod)' || NEW.siretype_cod),('(rowuuid)' || hex(NEW.rowuuid) || ',' ||'(siretype_des)' || hex(NEW.siretype_des) ));

END;

CREATE TRIGGER audit_6_delete
AFTER DELETE ON reg_lkpsiretype
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_lkpsiretype',('(siretype_cod)' || OLD.siretype_cod),('(rowuuid)' || hex(OLD.rowuuid) || ',' ||'(siretype_des)' || hex(OLD.siretype_des) ));

END;

CREATE TRIGGER audit_7_update
AFTER UPDATE ON reg_rpt_animreg
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','rowuuid',('(tagid)' || OLD.tagid),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','animalphoto',('(tagid)' || OLD.tagid),OLD.animalphoto,NEW.animalphoto);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','tagphoto',('(tagid)' || OLD.tagid),OLD.tagphoto,NEW.tagphoto);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','damcompoth',('(tagid)' || OLD.tagid),OLD.damcompoth,NEW.damcompoth);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','damcomp',('(tagid)' || OLD.tagid),OLD.damcomp,NEW.damcomp);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','dambreedoth',('(tagid)' || OLD.tagid),OLD.dambreedoth,NEW.dambreedoth);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','dambreed',('(tagid)' || OLD.tagid),OLD.dambreed,NEW.dambreed);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','dambdate',('(tagid)' || OLD.tagid),OLD.dambdate,NEW.dambdate);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','dambdknow',('(tagid)' || OLD.tagid),OLD.dambdknow,NEW.dambdknow);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','damid',('(tagid)' || OLD.tagid),OLD.damid,NEW.damid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sirecompoth',('(tagid)' || OLD.tagid),OLD.sirecompoth,NEW.sirecompoth);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sirecomp',('(tagid)' || OLD.tagid),OLD.sirecomp,NEW.sirecomp);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sirebreedoth',('(tagid)' || OLD.tagid),OLD.sirebreedoth,NEW.sirebreedoth);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sirebreed',('(tagid)' || OLD.tagid),OLD.sirebreed,NEW.sirebreed);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sirecnty',('(tagid)' || OLD.tagid),OLD.sirecnty,NEW.sirecnty);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sireid',('(tagid)' || OLD.tagid),OLD.sireid,NEW.sireid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','siretype',('(tagid)' || OLD.tagid),OLD.siretype,NEW.siretype);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','numparity',('(tagid)' || OLD.tagid),OLD.numparity,NEW.numparity);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','ccalvdate',('(tagid)' || OLD.tagid),OLD.ccalvdate,NEW.ccalvdate);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','fcalvdate',('(tagid)' || OLD.tagid),OLD.fcalvdate,NEW.fcalvdate);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','hascalved',('(tagid)' || OLD.tagid),OLD.hascalved,NEW.hascalved);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','deformitiesoth',('(tagid)' || OLD.tagid),OLD.deformitiesoth,NEW.deformitiesoth);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','deformities',('(tagid)' || OLD.tagid),OLD.deformities,NEW.deformities);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','description',('(tagid)' || OLD.tagid),OLD.description,NEW.description);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','composition_other',('(tagid)' || OLD.tagid),OLD.composition_other,NEW.composition_other);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','composition',('(tagid)' || OLD.tagid),OLD.composition,NEW.composition);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','breed_other',('(tagid)' || OLD.tagid),OLD.breed_other,NEW.breed_other);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','breed',('(tagid)' || OLD.tagid),OLD.breed,NEW.breed);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','sex',('(tagid)' || OLD.tagid),OLD.sex,NEW.sex);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','birthdate',('(tagid)' || OLD.tagid),OLD.birthdate,NEW.birthdate);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','birthknown',('(tagid)' || OLD.tagid),OLD.birthknown,NEW.birthknown);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','name',('(tagid)' || OLD.tagid),OLD.name,NEW.name);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','qid',('(tagid)' || OLD.tagid),OLD.qid,NEW.qid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg','tagid',('(tagid)' || OLD.tagid),OLD.tagid,NEW.tagid);

END;

CREATE TRIGGER audit_7_insert
AFTER INSERT ON reg_rpt_animreg
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_rpt_animreg',('(tagid)' || NEW.tagid),('(animalphoto)' || hex(NEW.animalphoto) || ',' ||'(birthdate)' || hex(NEW.birthdate) || ',' ||'(birthknown)' || hex(NEW.birthknown) || ',' ||'(breed)' || hex(NEW.breed) || ',' ||'(breed_other)' || hex(NEW.breed_other) || ',' ||'(ccalvdate)' || hex(NEW.ccalvdate) || ',' ||'(composition)' || hex(NEW.composition) || ',' ||'(composition_other)' || hex(NEW.composition_other) || ',' ||'(dambdate)' || hex(NEW.dambdate) || ',' ||'(dambdknow)' || hex(NEW.dambdknow) || ',' ||'(dambreed)' || hex(NEW.dambreed) || ',' ||'(dambreedoth)' || hex(NEW.dambreedoth) || ',' ||'(damcomp)' || hex(NEW.damcomp) || ',' ||'(damcompoth)' || hex(NEW.damcompoth) || ',' ||'(damid)' || hex(NEW.damid) || ',' ||'(deformities)' || hex(NEW.deformities) || ',' ||'(deformitiesoth)' || hex(NEW.deformitiesoth) || ',' ||'(description)' || hex(NEW.description) || ',' ||'(fcalvdate)' || hex(NEW.fcalvdate) || ',' ||'(hascalved)' || hex(NEW.hascalved) || ',' ||'(name)' || hex(NEW.name) || ',' ||'(numparity)' || hex(NEW.numparity) || ',' ||'(qid)' || hex(NEW.qid) || ',' ||'(rowuuid)' || hex(NEW.rowuuid) || ',' ||'(sex)' || hex(NEW.sex) || ',' ||'(sirebreed)' || hex(NEW.sirebreed) || ',' ||'(sirebreedoth)' || hex(NEW.sirebreedoth) || ',' ||'(sirecnty)' || hex(NEW.sirecnty) || ',' ||'(sirecomp)' || hex(NEW.sirecomp) || ',' ||'(sirecompoth)' || hex(NEW.sirecompoth) || ',' ||'(sireid)' || hex(NEW.sireid) || ',' ||'(siretype)' || hex(NEW.siretype) || ',' ||'(tagphoto)' || hex(NEW.tagphoto) ));

END;

CREATE TRIGGER audit_7_delete
AFTER DELETE ON reg_rpt_animreg
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_rpt_animreg',('(tagid)' || OLD.tagid),('(animalphoto)' || hex(OLD.animalphoto) || ',' ||'(birthdate)' || hex(OLD.birthdate) || ',' ||'(birthknown)' || hex(OLD.birthknown) || ',' ||'(breed)' || hex(OLD.breed) || ',' ||'(breed_other)' || hex(OLD.breed_other) || ',' ||'(ccalvdate)' || hex(OLD.ccalvdate) || ',' ||'(composition)' || hex(OLD.composition) || ',' ||'(composition_other)' || hex(OLD.composition_other) || ',' ||'(dambdate)' || hex(OLD.dambdate) || ',' ||'(dambdknow)' || hex(OLD.dambdknow) || ',' ||'(dambreed)' || hex(OLD.dambreed) || ',' ||'(dambreedoth)' || hex(OLD.dambreedoth) || ',' ||'(damcomp)' || hex(OLD.damcomp) || ',' ||'(damcompoth)' || hex(OLD.damcompoth) || ',' ||'(damid)' || hex(OLD.damid) || ',' ||'(deformities)' || hex(OLD.deformities) || ',' ||'(deformitiesoth)' || hex(OLD.deformitiesoth) || ',' ||'(description)' || hex(OLD.description) || ',' ||'(fcalvdate)' || hex(OLD.fcalvdate) || ',' ||'(hascalved)' || hex(OLD.hascalved) || ',' ||'(name)' || hex(OLD.name) || ',' ||'(numparity)' || hex(OLD.numparity) || ',' ||'(qid)' || hex(OLD.qid) || ',' ||'(rowuuid)' || hex(OLD.rowuuid) || ',' ||'(sex)' || hex(OLD.sex) || ',' ||'(sirebreed)' || hex(OLD.sirebreed) || ',' ||'(sirebreedoth)' || hex(OLD.sirebreedoth) || ',' ||'(sirecnty)' || hex(OLD.sirecnty) || ',' ||'(sirecomp)' || hex(OLD.sirecomp) || ',' ||'(sirecompoth)' || hex(OLD.sirecompoth) || ',' ||'(sireid)' || hex(OLD.sireid) || ',' ||'(siretype)' || hex(OLD.siretype) || ',' ||'(tagphoto)' || hex(OLD.tagphoto) ));

END;

CREATE TRIGGER audit_8_update
AFTER UPDATE ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg_msel_deformities','rowuuid',('(deformities)' || OLD.deformities|| ',' ||'(tagid)' || OLD.tagid),OLD.rowuuid,NEW.rowuuid);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg_msel_deformities','deformities',('(deformities)' || OLD.deformities|| ',' ||'(tagid)' || OLD.tagid),OLD.deformities,NEW.deformities);
INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_column,audit_key,audit_oldvalue,audit_newvalue) values (hex(datetime('now','localtime')),'UPDATE','NONE','reg_rpt_animreg_msel_deformities','tagid',('(deformities)' || OLD.deformities|| ',' ||'(tagid)' || OLD.tagid),OLD.tagid,NEW.tagid);

END;

CREATE TRIGGER audit_8_insert
AFTER INSERT ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'INSERT','NONE','reg_rpt_animreg_msel_deformities',('(deformities)' || NEW.deformities|| ',' ||'(tagid)' || NEW.tagid),('(rowuuid)' || hex(NEW.rowuuid) ));

END;

CREATE TRIGGER audit_8_delete
AFTER DELETE ON reg_rpt_animreg_msel_deformities
FOR EACH ROW BEGIN

INSERT INTO audit_log(audit_date,audit_action,audit_user,audit_table,audit_key,audit_insdeldata) values (hex(datetime('now','localtime')),'DELETE','NONE','reg_rpt_animreg_msel_deformities',('(deformities)' || OLD.deformities|| ',' ||'(tagid)' || OLD.tagid),('(rowuuid)' || hex(OLD.rowuuid) ));

END;

CREATE TRIGGER noAuditUpdates BEFORE UPDATE ON audit_log FOR EACH ROW BEGIN SELECT CASE WHEN ((SELECT NULL) IS NULL) THEN RAISE(ABORT, 'Audit table cannot be updated') END; END;

CREATE TRIGGER noAuditDeletes BEFORE DELETE ON audit_log FOR EACH ROW BEGIN SELECT CASE WHEN ((SELECT NULL) IS NULL) THEN RAISE(ABORT, 'Audit table cannot be deleted') END; END;

COMMIT;

