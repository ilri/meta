DROP TABLE IF EXISTS dict_lkpiso639;
DROP TABLE IF EXISTS dict_dctiso639;
DROP TABLE IF EXISTS dict_iso639;
DROP TABLE IF EXISTS audit_log ;
DROP TABLE IF EXISTS dict_childinfo ;
DROP TABLE IF EXISTS dict_relinfo ;
DROP TABLE IF EXISTS dict_clminfo ;
DROP TABLE IF EXISTS user_log ;
DROP TABLE IF EXISTS auth_user_user_permissions ;
DROP TABLE IF EXISTS auth_user_groups ;
DROP TABLE IF EXISTS auth_user ;
DROP TABLE IF EXISTS auth_group_permissions ;
DROP TABLE IF EXISTS auth_permission ;
DROP TABLE IF EXISTS dict_tblinfo ;
DROP TABLE IF EXISTS dict_grpinfo ;
DROP TABLE IF EXISTS auth_group ;

-- -----------------------------------------------------
-- Table dict_grpinfo
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_grpinfo (
  grp_cod VARCHAR(3) NOT NULL,
  grp_des VARCHAR(120) NULL DEFAULT NULL,
  PRIMARY KEY (grp_cod))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Groups. Used to group tables. Optional. Useful in big schema' /* comment truncated */ /*s*/;


-- -----------------------------------------------------
-- Table dict_tblinfo
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_tblinfo (
  scm_cod VARCHAR(60) NOT NULL,
  tbl_cod VARCHAR(60) NOT NULL,
  tbl_pos INT(11) NULL DEFAULT '-1',
  tbl_des TEXT NULL DEFAULT NULL,
  tbl_mdt TEXT NULL DEFAULT NULL,
  tbl_lkp TINYINT(1) NULL DEFAULT NULL,
  grp_cod VARCHAR(3) NULL DEFAULT NULL,
  PRIMARY KEY (tbl_cod, scm_cod),
  INDEX fk_tblinfo_grpinfo_idx (grp_cod ASC),
  CONSTRAINT fk_tblinfo_grpinfo
    FOREIGN KEY (grp_cod)
    REFERENCES dict_grpinfo (grp_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Information about the tables in the schema';


-- -----------------------------------------------------
-- Table dict_clminfo
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_clminfo (
  scm_cod VARCHAR(60) NOT NULL,
  tbl_cod VARCHAR(60) NOT NULL,
  clm_cod VARCHAR(60) NOT NULL,
  clm_pos INT(11) NULL DEFAULT NULL,
  clm_des TEXT NULL DEFAULT NULL,
  clm_mtd TEXT NULL DEFAULT NULL,
  clm_typ VARCHAR(60) NULL DEFAULT NULL,
  clm_key TINYINT(1) NULL DEFAULT NULL,
  clm_htmlwidth INT(11) NULL DEFAULT 150,
  clm_htmlalign VARCHAR(10) NULL DEFAULT 'left',
  clm_htmlsort VARCHAR(5) NULL DEFAULT 'true',
  clm_maindesc TINYINT(1) NULL DEFAULT 0,
  clm_protected TINYINT(1) NULL DEFAULT 0,
  PRIMARY KEY (scm_cod, tbl_cod, clm_cod),
  INDEX fk_dict_clminfo_dict_tblinfo1_idx (tbl_cod ASC, scm_cod ASC),
  CONSTRAINT fk_dict_clminfo_dict_tblinfo1
    FOREIGN KEY (tbl_cod , scm_cod)
    REFERENCES dict_tblinfo (tbl_cod , scm_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Information about each column in a table.';


-- -----------------------------------------------------
-- Table dict_relinfo
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_relinfo (
  scm_cod VARCHAR(60) NOT NULL,
  tbl_cod VARCHAR(60) NOT NULL,
  clm_cod VARCHAR(60) NOT NULL,
  rscm_cod VARCHAR(60) NOT NULL,
  rtbl_cod VARCHAR(60) NOT NULL,
  rclm_cod VARCHAR(60) NOT NULL,
  cnt_name VARCHAR(128) NULL,
  error_msg TEXT NULL,
  error_notes TEXT NULL,
  PRIMARY KEY (scm_cod, tbl_cod, clm_cod, rscm_cod, rtbl_cod, rclm_cod),
  CONSTRAINT fk_dict_relinfo_dict_clminfo1
    FOREIGN KEY (scm_cod , tbl_cod , clm_cod)
    REFERENCES dict_clminfo (scm_cod , tbl_cod , clm_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Stores the column realtionships between tables.';


-- -----------------------------------------------------
-- Table audit_log
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS audit_log (
  audit_id INT(11) NOT NULL AUTO_INCREMENT,
  audit_date TIMESTAMP NULL,
  audit_action VARCHAR(6) NULL,
  audit_user VARCHAR(120) NULL,
  audit_table VARCHAR(60) NULL,
  audit_column VARCHAR(60) NULL,
  audit_key VARCHAR(500) NULL,
  audit_oldvalue VARCHAR(500) NULL,
  audit_newvalue VARCHAR(500) NULL,
  audit_insdeldata TEXT NULL,
  PRIMARY KEY (audit_id))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Store the data changes made in each table';


-- -----------------------------------------------------
-- Table dict_iso639
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_iso639 (
  lang_cod VARCHAR(7) NOT NULL,
  lang_des VARCHAR(120) NULL,
  lang_def TINYINT NULL DEFAULT 0,
  PRIMARY KEY (lang_cod))
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Store the different languages supported in the schema.';


-- -----------------------------------------------------
-- Table dict_dctiso639
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_dctiso639 (
  trans_id INT NOT NULL AUTO_INCREMENT,
  lang_cod VARCHAR(7) NOT NULL,
  trans_des TEXT NULL,
  tscm_cod VARCHAR(60) NULL,
  ttbl_cod VARCHAR(60) NULL,
  cscm_cod VARCHAR(60) NULL,
  ctbl_cod VARCHAR(60) NULL,
  cclm_cod VARCHAR(60) NULL,
  PRIMARY KEY (trans_id),
  INDEX fk_dict_DCTISO639_ISO6391_idx (lang_cod ASC),
  INDEX fk_dict_dctiso639_dict_tblinfo1_idx (ttbl_cod ASC, tscm_cod ASC),
  INDEX fk_dict_dctiso639_dict_clminfo1_idx (cscm_cod ASC, ctbl_cod ASC, cclm_cod ASC),
  CONSTRAINT fk_dict_DCTISO639_ISO6391
    FOREIGN KEY (lang_cod)
    REFERENCES dict_iso639 (lang_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT fk_dict_dctiso639_dict_tblinfo1
    FOREIGN KEY (ttbl_cod , tscm_cod)
    REFERENCES dict_tblinfo (tbl_cod , scm_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT fk_dict_dctiso639_dict_clminfo1
    FOREIGN KEY (cscm_cod , ctbl_cod , cclm_cod)
    REFERENCES dict_clminfo (scm_cod , tbl_cod , clm_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Store the dictionary descriptions (Names of tables and colum' /* comment truncated */ /*ns) in different languages than the default.*/;


-- -----------------------------------------------------
-- Table dict_lkpiso639
-- -----------------------------------------------------
CREATE TABLE IF NOT EXISTS dict_lkpiso639 (
  scm_cod VARCHAR(60) NOT NULL,
  tbl_cod VARCHAR(60) NOT NULL,
  lang_cod VARCHAR(7) NOT NULL,
  lkp_value INT(11) NOT NULL,
  lkp_desc VARCHAR(120) NULL,
  PRIMARY KEY (scm_cod, tbl_cod, lang_cod, lkp_value),
  INDEX fk_dict_LKPISO369_ISO6391_idx (lang_cod ASC),
  INDEX fk_dict_lkpiso639_dict_tblinfo1_idx (tbl_cod ASC, scm_cod ASC),
  CONSTRAINT fk_dict_LKPISO369_ISO6391
    FOREIGN KEY (lang_cod)
    REFERENCES dict_iso639 (lang_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION,
  CONSTRAINT fk_dict_lkpiso639_dict_tblinfo1
    FOREIGN KEY (tbl_cod , scm_cod)
    REFERENCES dict_tblinfo (tbl_cod , scm_cod)
    ON DELETE CASCADE
    ON UPDATE NO ACTION)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8
COLLATE = utf8_unicode_ci
COMMENT = 'Store the lookup values of each lookup table in different la' /* comment truncated */ /*nguages than the default*/;