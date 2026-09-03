/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-09-20 13:18:50
Description: 获取配置数据函数
**************************************************/

#include "CUtils2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigItem(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

BM2_FUNCTION_EXPORT
int f_gctp_getConfigData(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CString configName = "";
	CString cfgWhere = "";

	list<CString> listCfgitmGrpName;

	CDataTable dtQuery;
	CDataTable dtColumn;

	EIClass bcls_rec_cfg;
	EIClass bcls_ret_cfg;

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	try
	{
		doFlag = f_gctp_getConfigItem(&bcls_rec_cfg, &bcls_ret_cfg, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		listCfgitmGrpName.push_back("DS_MAIN");
		listCfgitmGrpName.push_back("DS_ITEM");
		listCfgitmGrpName.push_back("CONDITION_MAIN");
		listCfgitmGrpName.push_back("CONDITION_ITEM");
		listCfgitmGrpName.push_back("GRID_MAIN");
		listCfgitmGrpName.push_back("GRID_ITEM");
		listCfgitmGrpName.push_back("CONTROL_MAIN");
		listCfgitmGrpName.push_back("CONTROL_ITEM");

		if (bcls_ret->Tables.Contains("FORM") && bcls_ret->Tables["FORM"].Rows.get_Count() > 0)
		{
			configName = bcls_ret->Tables["FORM"].Rows[0]["CFGITM_NAME"].ToString();
			cfgWhere = "WHERE cfgitm_name IN ('" + configName + "'";
			for (int i = 1; i < bcls_ret->Tables["FORM"].Rows.get_Count(); i++)
			{
				CString cfgitmName = bcls_ret->Tables["FORM"].Rows[i]["CFGITM_NAME"].ToString();
				cfgWhere += ",'" + cfgitmName + "'";

				sqlstr = "SELECT item_cvalue FROM tgctp01 WHERE cfgitm_name = '" + cfgitmName +
					"' AND item_ename = 'RELATE_CFGITEM_NAME' AND item_cvalue > ' '";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cfgWhere += ",'" + cmd_inq.GetString(1) + "'";
				}
				cmd_inq.Close();
			}
			cfgWhere += ")";
		}
		else if (bcls_rec->Tables.Contains("FORM") && bcls_rec->Tables["FORM"].Rows.get_Count() > 0)
		{
			configName = bcls_rec->Tables["FORM"].Rows[0]["CFGITM_NAME"].ToString();
			cfgWhere = "WHERE cfgitm_name IN ('" + configName + "'";
			for (int i = 1; i < bcls_rec->Tables["FORM"].Rows.get_Count(); i++)
			{
				CString cfgitmName = bcls_rec->Tables["FORM"].Rows[i]["CFGITM_NAME"].ToString();
				cfgWhere += ",'" + cfgitmName + "'";

				sqlstr = "SELECT item_cvalue FROM tgctp01 WHERE cfgitm_name = '" + cfgitmName +
					"' AND item_ename = 'RELATE_CFGITEM_NAME' AND item_cvalue > ' '";
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteReader();
				if (cmd_inq.Read())
				{
					cfgWhere += ",'" + cmd_inq.GetString(1) + "'";
				}
				cmd_inq.Close();
			}
			cfgWhere += ")";
		}
		else if (GetColValueC(bcls_rec->Tables[0],0,"CFGITM_NAME").Trim() != "")
		{
			configName = bcls_rec->Tables[0].Rows[0]["CFGITM_NAME"].ToString();
			cfgWhere = "WHERE cfgitm_name = '" + configName + "'";
		}
		else
		{
			strcpy(s.msg, "传入数据不完整");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		PrintLog("cfgWhere", cfgWhere);

		for (list<CString>::const_iterator iter = listCfgitmGrpName.begin(); iter != listCfgitmGrpName.end(); iter++)
		{
			CString cfgitm_grp_name = *iter;

			if (cfgitm_grp_name == "DS_ITEM")
			{
				bcls_ret->Tables.Add("DS_ITEM");
				list<CString> listCodeClass;

				sqlstr = "SELECT * FROM tgctp03 " + cfgWhere + " ORDER BY seq_no";
				PrintLog("sqlstr", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["DS_ITEM"]);
				cmd_inq.Close();

				//组合字段拆分处理
				for (int i = 0; i < bcls_ret_cfg.Tables["DS_ITEM"].Rows.get_Count(); i++)
				{	
					CString colName = bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["ITEM_ENAME"].ToString().Trim();
					CString dataType = bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["DATA_TYPE"].ToString();

					if (!bcls_ret->Tables["DS_ITEM"].Columns.Contains(colName))
					{
						//PrintLog("colName", colName);
						//PrintLog("dataType", dataType);

						if (dataType == "N")
						{
							bcls_ret->Tables["DS_ITEM"].Columns.Add(DT_DECIMAL, colName);
						}
						else
						{
							bcls_ret->Tables["DS_ITEM"].Columns.Add(DT_STRING, colName);
						}
					}

					if (bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["GROUP_RULE_NO"].ToString().Trim() != "" &&
						bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["GROUP_SEQ_NO"].ToDecimal() > 0)
					{
						CString grpColName = bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["GROUP_RULE_NO"].ToString().Trim();
						CString grpDivFlag = bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["DIV_FLAG"].ToString().Trim();
						int grpSeq = bcls_ret_cfg.Tables["DS_ITEM"].Rows[i]["GROUP_SEQ_NO"].ToDecimal().ToInt32();

						//PrintLog("grpColName", grpColName);
						//PrintLog("grpDivFlag", grpDivFlag);
						//PrintLog("grpSeq", grpSeq);

						for (int j = 0; j < bcls_ret->Tables["DS_ITEM"].Rows.get_Count(); j++)
						{
							CDataRow &drMainItem = bcls_ret->Tables["DS_ITEM"].Rows[j];

							if (grpDivFlag == "" && drMainItem[grpColName].ToString().GetLength() >= grpSeq)
							{
								drMainItem[colName] = drMainItem[grpColName].ToString().Substring(grpSeq - 1, 1);
							}
							else
							{
								list<CString> listColValue = StringSplit(drMainItem[grpColName].ToString(), grpDivFlag);
								int listPos = 0;
								for (list<CString>::const_iterator iter = listColValue.begin(); iter != listColValue.end(); iter++)
								{
									listPos++;
									if (listPos == grpSeq)
									{
										drMainItem[colName] = *iter;
										break;
									}
								}
							}
						}
					}
				}

				for (int i = 0; i < bcls_ret->Tables["DS_ITEM"].Rows.get_Count(); i++)
				{
					CString dsName = bcls_ret->Tables["DS_ITEM"].Rows[i]["CFGITM_NAME"].ToString();
					CString colName = bcls_ret->Tables["DS_ITEM"].Rows[i]["COLUMN_NAME"].ToString();
					CString tableName = bcls_ret->Tables["DS_ITEM"].Rows[i]["TABLE_NAME"].ToString();
					CString codeType = bcls_ret->Tables["DS_ITEM"].Rows[i]["CODE_TYPE"].ToString();
					CString blkName = "";

					if (codeType == "0")
					{
						continue;
					}
					else if (codeType == "1")
					{
						blkName = bcls_ret->Tables["DS_ITEM"].Rows[i]["CODE_CLASS"].ToString().Trim();
					}
					else
					{
						if (bcls_ret->Tables["DS_ITEM"].Rows[i]["CODE_CLASS"].ToString().Trim() != "")
						{
							blkName = bcls_ret->Tables["DS_ITEM"].Rows[i]["CODE_CLASS"].ToString().Trim();
						}
						else
						{
							blkName = dsName + "@" + colName;
						}
					}

					PrintLog("i", i);
					PrintLog("blkName", blkName);

					if (bcls_ret->Tables.Contains(blkName))
					{
						PrintLog("已存在,跳过");
						continue;
					}

					if (codeType == "1")
					{
						if (blkName != "")
						{
							bool existsFlag = false;
							for (list<CString>::const_iterator iter = listCodeClass.begin(); iter != listCodeClass.end(); iter++)
							{
								if (*iter == blkName)
								{
									existsFlag = true;
									break;
								}
							}

							if (existsFlag == false)
							{
								listCodeClass.push_back(blkName);
								PrintLog("push_back blkName", blkName);
							}
						}
					}
					else if (codeType == "2" &&
						bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
					{
						bcls_ret->Tables.Add(blkName);
						cmd_inq.SetCommandText(bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString());
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();

						if (bcls_ret->Tables[blkName].Columns.get_Count() > 1)
						{
							bcls_ret->Tables[blkName].Columns[0].set_ColumnName("CODE");
							bcls_ret->Tables[blkName].Columns[1].set_ColumnName("CODE_DESC_1_CONTENT");
						}
					}
					else if (codeType == "3")
					{
						bcls_ret->Tables.Add(blkName);
						bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE");
						bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");

						list<CString> listCodeSimple;
						CString strDatMapContent = bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						while (strDatMapContent.Find(';') > 0)
						{
							listCodeSimple.push_back(strDatMapContent.Substring(0, strDatMapContent.Find(';')));
							strDatMapContent = strDatMapContent.Substring(strDatMapContent.Find(';') + 1);
						}

						if (strDatMapContent.Trim() != "")
						{
							listCodeSimple.push_back(strDatMapContent.Trim());
						}

						if (listCodeSimple.size() > 0)
						{
							for (list<CString>::const_iterator iter = listCodeSimple.begin(); iter != listCodeSimple.end(); iter++)
							{
								bcls_ret->Tables[blkName].Rows.Add();
								CDataRow &drDataMap = bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1];
								CString strCodeDesc = *iter;
								if (strCodeDesc.Find(":") > 0)
								{
									drDataMap["CODE"] = strCodeDesc.Substring(0, strCodeDesc.Find(":"));
									if (drDataMap["CODE"].ToString().Trim() == "@")
									{
										drDataMap["CODE"] = " ";
									}
									drDataMap["CODE_DESC_1_CONTENT"] = strCodeDesc.Substring(strCodeDesc.Find(":") + 1);
								}
								else
								{
									if (strCodeDesc.Trim() == "@")
									{
										drDataMap["CODE"] = " ";
										drDataMap["CODE_DESC_1_CONTENT"] = " ";
									}
									else
									{
										drDataMap["CODE"] = strCodeDesc;
										drDataMap["CODE_DESC_1_CONTENT"] = strCodeDesc;
									}
								}
							}
						}
					}
					else if (codeType == "4" && !bcls_ret->Tables.Contains("USERID"))
					{
						bcls_ret->Tables.Add("USERID");
						PrintLog("USERID");
						sqlstr = "SELECT ename AS CODE,cname AS CODE_DESC_1_CONTENT FROM tesuserinfo WHERE ename <> 'admin' AND isenable = 1";
						cmd_inq.SetCommandText(sqlstr);
						cmd_inq.ExecuteQuery(bcls_ret->Tables["USERID"]);
						cmd_inq.Close();

						bcls_ret->Tables["USERID"].Rows.Add();
						bcls_ret->Tables["USERID"].Rows[bcls_ret->Tables["USERID"].Rows.get_Count() - 1]["CODE"] = "admin";
						bcls_ret->Tables["USERID"].Rows[bcls_ret->Tables["USERID"].Rows.get_Count() - 1]["CODE_DESC_1_CONTENT"] = "管理员";
					}
					else if (codeType == "5")
					{
						bcls_ret->Tables.Add(blkName);
						CString codeSql = "SELECT unit_code AS code,unit_cname AS code_desc_1_content,whole_backlog_code,mat_line_type,'0' AS default_flag FROM tmm00si16 WHERE 1 = 1";

						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
						{
							codeSql += " AND " + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						}

						codeSql += " ORDER BY unit_code";
						PrintLog("机组查询codeSql", codeSql);
						cmd_inq.SetCommandText(codeSql);
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();

						//PrintDataTable(bcls_ret->Tables[blkName]);
					}
					else if (codeType == "6")
					{
						if (!bcls_ret->Tables.Contains(blkName))
						{
							bcls_ret->Tables.Add(blkName);
						}
						CString codeSql = "SELECT whole_backlog_code AS code,whole_backlog_name AS code_desc_1_content FROM tsi0001"
							" WHERE backlog_type = '10' AND whole_backlog_code NOT LIKE 'B%'";

						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
						{
							codeSql += " AND " + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						}

						codeSql += " ORDER BY whole_backlog_code";
						PrintLog("codeSql", codeSql);
						cmd_inq.SetCommandText(codeSql);
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();

						bcls_ret->Tables[blkName].Rows.Add();
						bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1]["CODE"] = "9A";
						bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1]["CODE_DESC_1_CONTENT"] = "准发";
						bcls_ret->Tables[blkName].Rows.Add();
						bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1]["CODE"] = "9B";
						bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1]["CODE_DESC_1_CONTENT"] = "发货";
					}
					else if (codeType == "7")
					{
						bcls_ret->Tables.Add(blkName);
						CString codeSql = "SELECT DISTINCT " + colName;
						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["CODE_BIND_ENAME1"].ToString().Trim() != "")
						{
							codeSql += "," + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["CODE_BIND_ENAME1"].ToString().Trim();
						}

						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["CODE_BIND_ENAME2"].ToString().Trim() != "")
						{
							codeSql += "," + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["CODE_BIND_ENAME2"].ToString().Trim();
						}

						codeSql += " FROM " + tableName + " ORDER BY " + colName;
						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
						{
							codeSql += " AND " + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						}
						cmd_inq.SetCommandText(codeSql);
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();

						bcls_ret->Tables[blkName].Columns[0].set_ColumnName("CODE");
						if (bcls_ret->Tables[blkName].Columns.get_Count() > 1)
						{
							bcls_ret->Tables[blkName].Columns[1].set_ColumnName("CODE_DESC_1_CONTENT");
						}
					}
					else if (codeType == "9" && !bcls_ret->Tables.Contains("UNIT_CODE_USER"))
					{
						blkName = "UNIT_CODE_USER";
						bcls_ret->Tables.Add(blkName);
						CDataTable dtUnitCode;
						list<CString> listUserGroup;
						int adminFlag = 0;

						PrintLog("UNIT_CODE_USER");

						if ((CString)s.userid == "admin" || (CString)s.userid == "178773")
						{
							adminFlag = 1;
						}
						else
						{
							sqlstr = "SELECT b.name FROM ES.TESGROUPMEMBER a,ES.TESGROUPINFO b, ES.TESUSERINFO c"
								" WHERE a.groupid = b.id AND a.memberid = c.id AND c.ename = '" + (CString)s.userid + "'";
							PrintLog("sqlstr", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteReader();
							while (cmd_inq.Read())
							{
								if (cmd_inq.GetString(1) == "admingroup" || cmd_inq.GetString(1) == "PGSC1A")
								{
									adminFlag = 1;
									break;
								}

								PrintLog("PUSH", cmd_inq.GetString(1));
								listUserGroup.push_back(cmd_inq.GetString(1));
							}
							cmd_inq.Close();
						}

						PrintLog("adminFlag", adminFlag);

						CString codeSql = "SELECT unit_code AS code,unit_cname AS code_desc_1_content,equ_no AS 设备号, whole_backlog_code,"
							"mat_line_type,'0' AS default_flag,group_key_value,device_name FROM tmm00si16 WHERE equ_no > ' '";

						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
						{
							codeSql += " AND " + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						}

						if (adminFlag == 0)
						{
							//codeSql += " AND device_name IN ('" + (CString)s.fore_machine + "',' ')";
						}

						codeSql += " ORDER BY unit_code";
						PrintLog("codeSql", codeSql);
						cmd_inq.SetCommandText(codeSql);

						if (adminFlag == 1)
						{
							cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						}
						else
						{
							cmd_inq.ExecuteQuery(dtUnitCode);
						}
						cmd_inq.Close();

						if (adminFlag == 0)
						{
							for (int j = 0; j < dtUnitCode.Rows.get_Count(); j++)
							{
								PrintLog("GROUP_KEY_VALUE", dtUnitCode.Rows[j]["GROUP_KEY_VALUE"].ToString());
								list<CString> listOperGrp = StringSplit(dtUnitCode.Rows[j]["GROUP_KEY_VALUE"].ToString(), "/");

								for (list<CString>::const_iterator iter = listUserGroup.begin(); iter != listUserGroup.end(); iter++)
								{
									for (list<CString>::const_iterator iterSub = listOperGrp.begin(); iterSub != listOperGrp.end(); iterSub++)
									{
										if (*iterSub == *iter)
										{
											int unitExistsFlag = 0;
											for (int k = 0; k < bcls_ret->Tables[blkName].Rows.get_Count(); k++)
											{
												if (bcls_ret->Tables[blkName].Rows[k]["CODE"].ToString() == dtUnitCode.Rows[j]["CODE"].ToString())
												{
													unitExistsFlag = 1;
													break;
												}
											}

											if (unitExistsFlag == 0)
											{
												bcls_ret->Tables[blkName].Rows.Add();
												CDataRow& dr = bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1];
												MergDataRow(dtUnitCode.Rows[j], dr, true, true);
											}

											break;
										}
									}
								}
							}
						}
						
						//PrintDataTable(bcls_ret->Tables[blkName]);
						if (bcls_ret->Tables[blkName].Rows.get_Count() > 1)
						{
							CString strIn = "(";
							for (int j = 0; j < bcls_ret->Tables[blkName].Rows.get_Count(); j++)
							{
								bcls_ret->Tables[blkName].Rows[j]["DEFAULT_FLAG"] = "0";
								if (j > 0)
								{
									strIn += ",";
								}

								strIn += "'" + bcls_ret->Tables[blkName].Rows[j]["CODE"].ToString() + "'";
							}
							strIn += ")";

							CDataTable dtShift;
							sqlstr = "SELECT * FROM tmm0010 WHERE device_id = '" + (CString)s.fore_machine +
								"' AND unit_code IN " + strIn + " ORDER BY prod_seq_no DESC";
							PrintLog("sqlstr", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteQuery(dtShift);
							cmd_inq.Close();

							if (dtShift.Rows.get_Count() > 0)
							{
								int defaultFlag = 0;
								for (int j = 0; j < bcls_ret->Tables[blkName].Rows.get_Count(); j++)
								{
									if (bcls_ret->Tables[blkName].Rows[j]["CODE"].ToString() == dtShift.Rows[0]["UNIT_CODE"].ToString())
									{
										bcls_ret->Tables[blkName].Rows[j]["DEFAULT_FLAG"] = "1";
										defaultFlag = 1;
									}
								}

								if (defaultFlag == 0)
								{
									bcls_ret->Tables[blkName].Rows[0]["DEFAULT_FLAG"] = "1";
								}
							}
							else
							{
								bcls_ret->Tables[blkName].Rows[0]["DEFAULT_FLAG"] = "1";
							}

							//PrintDataTable(bcls_ret->Tables[blkName]);
						}
					}
					else if (codeType == "A" && !bcls_ret->Tables.Contains("UNIT_CODE_FORM"))
					{
						blkName = "UNIT_CODE_FORM";
						bcls_ret->Tables.Add(blkName);

						CString codeSql = "";
						if (((CString)s.formname).Find("B1") > 0)
						{
							CString strForm = "T" + ((CString)s.formname).Replace("B1", "");
							codeSql = "SELECT unit_code AS code,unit_cname AS code_desc_1_content,whole_backlog_code,mat_line_type,'0' AS default_flag FROM tmm00si16"
								" WHERE prod_table_name = '" + strForm + "' ORDER BY unit_code";
						}
						else
						{
							codeSql = "SELECT unit_code AS code,unit_cname AS code_desc_1_content,whole_backlog_code,mat_line_type,'0' AS default_flag FROM tmm00si16"
								" WHERE form_no = '" + (CString)s.formname + "' OR form_code = '" + (CString)s.formname + "' OR form_code || 'P' = '" + 
								(CString)s.formname + "' ORDER BY unit_code";
						}
						
						PrintLog("机组查询codeSql", codeSql);
						cmd_inq.SetCommandText(codeSql);
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();
					}
					/*
					else if (codeType == "A" && !bcls_ret->Tables.Contains("UNIT_CODE_COMPUTER"))
					{
						blkName = "UNIT_CODE_COMPUTER";
						bcls_ret->Tables.Add(blkName);

						bcls_ret->Tables.Add(blkName);
						CDataTable dtUnitCode;
						int adminFlag = 0;

						if ((CString)s.userid == "admin" || (CString)s.userid == "178773")
						{
							adminFlag = 1;
						}
						else
						{
							sqlstr = "SELECT b.name FROM ES.TESGROUPMEMBER a,ES.TESGROUPINFO b, ES.TESUSERINFO c"
								" WHERE a.groupid = b.id AND a.memberid = c.id AND c.ename = '" + (CString)s.userid + "'";
							PrintLog("sqlstr", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteReader();
							while (cmd_inq.Read())
							{
								if (cmd_inq.GetString(1) == "admingroup")
								{
									adminFlag = 1;
									break;
								}

								PrintLog("PUSH", cmd_inq.GetString(1));
							}
							cmd_inq.Close();
						}

						PrintLog("adminFlag", adminFlag);

						CString codeSql = "SELECT unit_code AS code,unit_cname AS code_desc_1_content,equ_no AS 设备号, whole_backlog_code,"
							"mat_line_type,'0' AS default_flag,group_key_value FROM tmm00si16 WHERE equ_no > ' '";

						if (bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim() != "")
						{
							codeSql += " AND " + bcls_ret->Tables[cfgitm_grp_name].Rows[i]["SQL_CONTEXT"].ToString().Trim();
						}

						if (adminFlag == 0)
						{
							codeSql += " AND device_name = '" + (CString)s.fore_machine + "'";
						}

						codeSql += " ORDER BY unit_code";
						PrintLog("codeSql", codeSql);
						cmd_inq.SetCommandText(codeSql);
						cmd_inq.ExecuteQuery(bcls_ret->Tables[blkName]);
						cmd_inq.Close();

						//PrintDataTable(bcls_ret->Tables[blkName]);
						if (bcls_ret->Tables[blkName].Rows.get_Count() > 0)
						{
							CString strIn = "(";
							for (int j = 0; j < bcls_ret->Tables[blkName].Rows.get_Count(); j++)
							{
								bcls_ret->Tables[blkName].Rows[j]["DEFAULT_FLAG"] = "0";
								if (j > 0)
								{
									strIn += ",";
								}

								strIn += "'" + bcls_ret->Tables[blkName].Rows[j]["CODE"].ToString() + "'";
							}
							strIn += ")";

							CDataTable dtShift;
							sqlstr = "SELECT * FROM tmm0010 WHERE device_id = '" + (CString)s.fore_machine +
								"' AND unit_code IN " + strIn + " ORDER BY prod_seq_no DESC";
							PrintLog("sqlstr", sqlstr);
							cmd_inq.SetCommandText(sqlstr);
							cmd_inq.ExecuteQuery(dtShift);
							cmd_inq.Close();

							if (dtShift.Rows.get_Count() > 0)
							{
								int defaultFlag = 0;
								for (int j = 0; j < bcls_ret->Tables[blkName].Rows.get_Count(); j++)
								{
									if (bcls_ret->Tables[blkName].Rows[j]["CODE"].ToString() == dtShift.Rows[0]["UNIT_CODE"].ToString())
									{
										bcls_ret->Tables[blkName].Rows[j]["DEFAULT_FLAG"] = "1";
										defaultFlag = 1;
									}
								}

								if (defaultFlag == 0)
								{
									bcls_ret->Tables[blkName].Rows[0]["DEFAULT_FLAG"] = "1";
								}
							}
							else
							{
								bcls_ret->Tables[blkName].Rows[0]["DEFAULT_FLAG"] = "1";
							}

							//PrintDataTable(bcls_ret->Tables[blkName]);
						}
					}
					*/
				}

				if (listCodeClass.size() > 0)
				{
					CDataTable dtCodeMap;
					CDataTable dtCodeClass;

					CString sqlstr1 = "SELECT code_class,code,code_desc_1_content,code_desc_2_content,code_desc_3_content,code_desc_4_content"
						",code_desc_5_content FROM tep0002 WHERE code_class IN (";
					CString sqlstr2 = "SELECT code_class,code_name,code_desc_1_name,code_desc_2_name,code_desc_3_name,code_desc_4_name,code_desc_5_name"
						" FROM tep0001 WHERE code_class IN (";

					for (list<CString>::const_iterator iter = listCodeClass.begin(); iter != listCodeClass.end(); iter++)
					{
						if (iter != listCodeClass.begin())
						{
							sqlstr1 += ",";
							sqlstr2 += ",";
						}
						sqlstr1 += "'" + *iter + "'";
						sqlstr2 += "'" + *iter + "'";
					}
					sqlstr1 += ") ORDER BY code_class,code";
					PrintLog("sqlstr1", sqlstr1);
					cmd_inq.SetCommandText(sqlstr1);
					cmd_inq.ExecuteQuery(dtCodeMap);
					cmd_inq.Close();

					sqlstr2 += ") ORDER BY code_class";
					cmd_inq.SetCommandText(sqlstr2);
					cmd_inq.ExecuteQuery(dtCodeClass);
					cmd_inq.Close();

					CString blkName = "";
					for (int i = 0; i < dtCodeMap.Rows.get_Count(); i++)
					{
						if (!bcls_ret->Tables.Contains(dtCodeMap.Rows[i]["CODE_CLASS"].ToString()))
						{
							blkName = dtCodeMap.Rows[i]["CODE_CLASS"].ToString();
							bcls_ret->Tables.Add(blkName);
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE");
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_1_CONTENT");
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_2_CONTENT");
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_3_CONTENT");
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_4_CONTENT");
							bcls_ret->Tables[blkName].Columns.Add(DT_STRING, "CODE_DESC_5_CONTENT");

							for (int j = 0; j < dtCodeClass.Rows.get_Count(); j++)
							{
								if (dtCodeClass.Rows[j]["CODE_CLASS"].ToString() == blkName)
								{
									bcls_ret->Tables[blkName].Columns["CODE"].set_Caption(dtCodeClass.Rows[j]["CODE_NAME"].ToString());
									bcls_ret->Tables[blkName].Columns["CODE_DESC_1_CONTENT"].set_Caption(dtCodeClass.Rows[j]["CODE_DESC_1_NAME"].ToString());
									bcls_ret->Tables[blkName].Columns["CODE_DESC_2_CONTENT"].set_Caption(dtCodeClass.Rows[j]["CODE_DESC_2_NAME"].ToString());
									bcls_ret->Tables[blkName].Columns["CODE_DESC_3_CONTENT"].set_Caption(dtCodeClass.Rows[j]["CODE_DESC_3_NAME"].ToString());
									bcls_ret->Tables[blkName].Columns["CODE_DESC_4_CONTENT"].set_Caption(dtCodeClass.Rows[j]["CODE_DESC_4_NAME"].ToString());
									bcls_ret->Tables[blkName].Columns["CODE_DESC_5_CONTENT"].set_Caption(dtCodeClass.Rows[j]["CODE_DESC_5_NAME"].ToString());
									break;
								}
							}
						}

						bcls_ret->Tables[blkName].Rows.Add();
						bcls_ret->Tables[blkName].Rows[bcls_ret->Tables[blkName].Rows.get_Count() - 1].Merge(dtCodeMap.Rows[i]);
					}
				}
			}
			else if (cfgitm_grp_name == "CONDITION_ITEM")
			{
				bcls_ret->Tables.Add("CONDITION_ITEM");

				sqlstr = "SELECT * FROM tgctp02 " + cfgWhere + " AND cfgitm_grp_name = 'CONDITION_ITEM' ORDER BY seq_no_01";
				PrintLog("sqlstr", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["CONDITION_ITEM"]);
				cmd_inq.Close();

				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_SEQ_NO");
				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_ROW_NO");
				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_COL_NO");
				for (int j = 0; j < bcls_ret->Tables["CONDITION_ITEM"].Rows.get_Count(); j++)
				{
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_SEQ_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["SEQ_NO_01"];
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_ROW_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["LOCATION_X"];
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_COL_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["LOCATION_Y"];
				}
			}
			else if (cfgitm_grp_name == "GRID_ITEM")
			{
				bcls_ret->Tables.Add("GRID_ITEM");

				sqlstr = "SELECT * FROM tgctp02 " + cfgWhere + " AND cfgitm_grp_name = 'GRID_ITEM' ORDER BY seq_no_02";
				PrintLog("sqlstr", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["GRID_ITEM"]);
				cmd_inq.Close();
			}
			else if (cfgitm_grp_name == "CONTROL_ITEM")
			{
				bcls_ret->Tables.Add("CONTROL_ITEM");

				sqlstr = "SELECT * FROM tgctp02 " + cfgWhere + " AND cfgitm_grp_name = 'CONTROL_ITEM' ORDER BY seq_no_03";
				PrintLog("sqlstr", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(bcls_ret->Tables["CONTROL_ITEM"]);
				cmd_inq.Close();

				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_SEQ_NO");
				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_ROW_NO");
				bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, "CONTROL_COL_NO");

				for (int j = 0; j < bcls_ret->Tables["CONTROL_ITEM"].Rows.get_Count(); j++)
				{
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_SEQ_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["SEQ_NO_03"];
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_ROW_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["ROW_SEQ"];
					bcls_ret->Tables[cfgitm_grp_name].Rows[j]["CONTROL_COL_NO"] = bcls_ret->Tables[cfgitm_grp_name].Rows[j]["COL_SEQ"];
				}
			}
			else
			{
				//查询配置表主项数据
				sqlstr = "SELECT * FROM tgctp01 " + cfgWhere + " AND cfgitm_grp_name = '" + cfgitm_grp_name + "' ORDER BY now_row";
				PrintLog("sqlstr", sqlstr);
				cmd_inq.SetCommandText(sqlstr);
				cmd_inq.ExecuteQuery(dtQuery);
				cmd_inq.Close();

				//PrintDataTable(dtQuery);
				if (dtQuery.Rows.get_Count() > 0)
				{
					if (!bcls_ret->Tables.Contains(cfgitm_grp_name))
					{
						bcls_ret->Tables.Add(cfgitm_grp_name);
					}
					bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_STRING, "CFGITM_NAME");

					dtColumn = bcls_ret_cfg.Tables[cfgitm_grp_name];

					for (int i = 0; i < dtQuery.Rows.get_Count(); i++)
					{
						if (i == 0 || dtQuery.Rows[i]["NOW_ROW"].ToString() != dtQuery.Rows[i - 1]["NOW_ROW"].ToString())
						{
							bcls_ret->Tables[cfgitm_grp_name].Rows.Add();
						}

						CDataRow& drRet = bcls_ret->Tables[cfgitm_grp_name].Rows[bcls_ret->Tables[cfgitm_grp_name].Rows.get_Count() - 1];
						drRet["CFGITM_NAME"] = dtQuery.Rows[i]["CFGITM_NAME"];

						for (int j = 0; j < dtColumn.Rows.get_Count(); j++)
						{
							CString colName = dtColumn.Rows[j]["ITEM_ENAME"].ToString();
							CString colCaption = dtColumn.Rows[j]["ITEM_CNAME"].ToString();
							CString colDataType = dtColumn.Rows[j]["DATA_TYPE"].ToString();

							if (!bcls_ret->Tables[cfgitm_grp_name].Columns.Contains(colName))
							{
								if (colDataType == "C" || colDataType == "T")
								{
									bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_STRING, colName);
								}
								else
								{
									bcls_ret->Tables[cfgitm_grp_name].Columns.Add(DT_DECIMAL, colName);
								}

								bcls_ret->Tables[cfgitm_grp_name].Columns[colName].set_Caption(colCaption);
							}

							if (dtQuery.Rows[i]["ITEM_ENAME"].ToString() == colName)
							{
								if (colDataType == "C" || colDataType == "T")
								{
									drRet[colName] = dtQuery.Rows[i]["ITEM_CVALUE"].ToString();
								}
								else
								{
									drRet[colName] = dtQuery.Rows[i]["ITEM_CVALUE"].ToDecimal();
								}

								break;
							}
						}
					}

					for (int i = 0; i < dtColumn.Rows.get_Count(); i++)
					{
						CString colName = dtColumn.Rows[i]["ITEM_ENAME"].ToString();
						CString colCaption = dtColumn.Rows[i]["ITEM_CNAME"].ToString();
						CString colDataType = dtColumn.Rows[i]["DATA_TYPE"].ToString();
						CString colDefaultValue = dtColumn.Rows[i]["DEFAULT_VALUE"].ToString().Trim();

						for (int j = 0; j < bcls_ret->Tables[cfgitm_grp_name].Rows.get_Count(); j++)
						{
							if (colDataType == "N" || colDataType == "B")
							{
								if (!bcls_ret->Tables[cfgitm_grp_name].Columns.Contains(colName) ||
									bcls_ret->Tables[cfgitm_grp_name].Rows[j][colName].ToString().Trim() == "")
								{
									if (colDefaultValue == "")
									{
										AddColValue(bcls_ret->Tables[cfgitm_grp_name], j, colName, (CDecimal)0);
									}
									else
									{
										AddColValue(bcls_ret->Tables[cfgitm_grp_name], j, colName, dtColumn.Rows[i]["DEFAULT_VALUE"].ToDecimal());
									}
								}
							}
							else if (!bcls_ret->Tables[cfgitm_grp_name].Columns.Contains(colName))
							{
								AddColValue(bcls_ret->Tables[cfgitm_grp_name], j, colName, dtColumn.Rows[i]["DEFAULT_VALUE"].ToString());
							}
						}
					}

					if (cfgitm_grp_name == "CONTROL_MAIN" &&
						bcls_ret->Tables[cfgitm_grp_name].Rows[0]["ADD_CONTROL_METHOD"].ToString() == "2" &&
						bcls_ret->Tables[cfgitm_grp_name].Rows[0]["ED54_FUNCTION_ID"].ToString().Trim() != "")
					{
						CString funcId = bcls_ret->Tables[cfgitm_grp_name].Rows[0]["ED54_FUNCTION_ID"].ToString().Trim();
						PrintLog("funcId", funcId);

						switch (conn->DatabaseKind)
						{
						case DB_KIND_MSSQL:	        // MS SQL Server数据库
						case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
						case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
							sqlstr = "SELECT MAX(row_count) FROM (SELECT a.func_id,a.class_code,CEILING(b.item_count/a.column_count) row_count"
								" FROM ted53 a,(SELECT func_id, class_code, count(1) AS item_count FROM ted54 WHERE item_hide_flag NOT IN ('1','3')"
								" GROUP BY func_id, class_code) b WHERE a.func_id = b.func_id AND a.class_code = b.class_code) WHERE func_id = '" +
								funcId + "' GROUP BY FUNC_ID";
							break;
						case DB_KIND_ORACLE:	    // Oracle 数据库
						default:
							sqlstr = "SELECT MAX(row_count) FROM (SELECT a.func_id,a.class_code,CEIL(b.item_count/a.column_count) row_count"
								" FROM ted53 a,(SELECT func_id, class_code, count(1) AS item_count FROM ted54 WHERE item_hide_flag NOT IN ('1','3')"
								" GROUP BY func_id, class_code) b WHERE a.func_id = b.func_id AND a.class_code = b.class_code) WHERE func_id = '" +
								funcId + "' GROUP BY FUNC_ID";
							break;
						}

						CDbCommand cmd_inq(conn);
						cmd_inq.SetCommandText(sqlstr);
						CDecimal rowCount = cmd_inq.ExecuteScalar();

						AddColValue(bcls_ret->Tables[cfgitm_grp_name], 0, "ROW_NUM", rowCount);
					}
				}
			}
		}
	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	//if (doFlag == 0)
	//{
	//	CTransactionManager::Commit(0);
	//	CTransactionManager::Begin(0, 0);
	//}
	//else
	//{
	//	CTransactionManager::Abort(0);
	//	CTransactionManager::Begin(0, 0);
	//}

	return doFlag;
}
