#include "CDynaTable2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigItem(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmGrpName, CString custFlag);

BM2_FUNCTION_IMPORT
int f_gctp_getConfigData(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_IMPORT
int f_gctp_buttonConfig(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString formNo);

BM2_FUNCTION_EXPORT
int f_gctp_setFromConfig(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量	
	CString formNo = "";
	CString sOperateRow = "";
	int iRowNo = 0;

	//动态数据表定义
	CDynaTable2 tgctp04("TGCTP04", conn);
	CDynaTable2 tgctp05("TGCTP05", conn);
	CDynaTable2 tgctp06("TGCTP06", conn);
	CDynaTable2 tgctp07("TGCTP07", conn);

	//数据库操作类定义
	CDbCommand cmd(conn);
	CDbCommand cmd_upd(conn);

	try
	{
		for (int i = 0; i < bcls_rec->Tables.get_Count(); i++)
		{
			CString blkName = bcls_rec->Tables[i].get_TableName();
			PrintLog("新增配置数据,blkName", blkName);

			if (blkName == "FORM" || blkName == "FORM_CFGITM")
			{
				CString sqlWhere = " AND function_id NOT IN (";
				for (int j = 0; j < bcls_rec->Tables[blkName].Rows.get_Count(); j++)
				{
					tgctp04.MergeFrom(bcls_rec->Tables[blkName].Rows[j]);
					if (bcls_rec->Tables[blkName].Rows[j]["FUNCTION_ID"].ToString().Trim() == "")
					{
						tgctp04.SetColVal("FUNCTION_ID", CDateTime::Now().ToString("yyMMdd") + GetSeqence("GCTP_SEQ_ID", 4, conn));
						if (tgctp04.Insert() < 0)
						{
							throw CApplicationException(-1, s.msg, s.svc_name);
						}
					}
					else
					{
						CString formName = "";
						CString cfgitmName = "";
						sqlstr = "SELECT form_name,cfgitm_name FROM tgctp04 WHERE function_id = '" + tgctp04.GetColValString("FUNCTION_ID") + "'";
						cmd.SetCommandText(sqlstr);
						cmd.ExecuteReader();
						if (cmd.Read())
						{
							if (tgctp04.Delete() < 0)
							{
								throw CApplicationException(-1, s.msg, log.Location);
							}

							cfgitmName = cmd.GetString(2);
							if (tgctp04.GetColValString("CFGITM_NAME") != cfgitmName)
							{
								PrintLog("cfgitmName", cfgitmName);
								PrintLog("更改操作配置中原配置名为新配置名", tgctp04.GetColValString("CFGITM_NAME"));

								sqlstr = "UPDATE tgctp05 SET dataset_name = @CFGITM_NAME_NEW WHERE form_no = @FORM_NO AND dataset_name = @CFGITM_NAME_OLD";
								cmd_upd.Parameters.Set("FORM_NO", tgctp04.GetColValString("FORM_NO"));
								cmd_upd.Parameters.Set("CFGITM_NAME_NEW", tgctp04.GetColValString("CFGITM_NAME"));
								cmd_upd.Parameters.Set("CFGITM_NAME_OLD", cfgitmName);
								cmd_upd.SetCommandText(sqlstr);
								cmd_upd.ExecuteNonQuery();
								cmd_upd.Close();

								sqlstr = "UPDATE tgctp06 SET cfgitm_name = @CFGITM_NAME_NEW WHERE form_no = @FORM_NO AND cfgitm_name = @CFGITM_NAME_OLD";
								cmd_upd.SetCommandText(sqlstr);
								cmd_upd.ExecuteNonQuery();
								cmd_upd.Close();
							}

							if (tgctp04.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						else
						{
							tgctp04.SetColVal("FUNCTION_ID", CDateTime::Now().ToString("yyMMdd") + GetSeqence("GCTP_SEQ_ID", 4, conn));
							if (tgctp04.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
						cmd.Close();
					}

					if (j > 0)
					{
						sqlWhere += ",";
					}
					sqlWhere += "'" + tgctp04.GetColValString("FUNCTION_ID") + "'";
				}

				if (bcls_rec->Tables[blkName].Rows.get_Count() > 0)
				{
					sqlstr = "DELETE FROM tgctp04 WHERE form_no = '" + tgctp04.GetColValString("FORM_NO") + "'" + sqlWhere + ")";
					PrintLog("删除不存在的,sqlstr", sqlstr);
					cmd.SetCommandText(sqlstr);
					cmd.ExecuteNonQuery();
					cmd.Close();
				}
			}
			else if (blkName == "CUSTOM_EVENT")
			{
				if (formNo.Trim() == "")
				{
					formNo = tgctp04.GetColValString("FORM_NO");
				}

				if (formNo.Trim() == "")
				{
					formNo = GetColValueC(bcls_rec->Tables[0], 0, "FORM_NO");
				}

				PrintLog("formNo", formNo);

				sqlstr = "DELETE FROM tgctp07 WHERE form_no = '" + formNo + "'";
				cmd.SetCommandText(sqlstr);
				cmd.ExecuteNonQuery();
				cmd.Close();

				for (int j = 0; j < bcls_rec->Tables[blkName].Rows.get_Count(); j++)
				{
					CDecimal buttonId = GetColValueD(bcls_rec->Tables[blkName], j, "ID");
					//CString name = GetColValueC(bcls_rec->Tables[blkName], j, "NAME");
					//CString buttonDesc = GetColValueC(bcls_rec->Tables[blkName], j, "DESCRIPTION");
					CString operateType = GetColValueC(bcls_rec->Tables[blkName], j, "OPERATE_TYPE");

					if (buttonId == 0)
					{
						buttonId = 2000000000 + GetSeqence("GCTP_EVENT_SEQ_NO", conn);

						if (operateType == "11" && tgctp04.GetRowCount() > 1)
						{
							CString mainCfgitemName = "";
							CString subCfgitemName = "";
							for (int k = 0; k < tgctp04.GetRowCount(); k++)
							{
								if (tgctp04.GetColValString("MAIN_FLAG", k) == "1")
								{
									mainCfgitemName = tgctp04.GetColValString("CFGITM_NAME", k).Trim();
								}
								else
								{
									subCfgitemName = tgctp04.GetColValString("CFGITM_NAME", k).Trim();
								}
							}

							if (mainCfgitemName == "")
							{
								mainCfgitemName = tgctp04.GetColValString("CFGITM_NAME").Trim();
							}

							PrintLog("mainCfgitemName", mainCfgitemName);
							PrintLog("subCfgitemName", subCfgitemName);

							//操作配置表数据新增
							tgctp05.SetColVal("ID", buttonId);
							tgctp05.SetColVal("SEQ_NO", (CDecimal)1);
							tgctp05.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
							tgctp05.SetColVal("DATASET_NAME", tgctp04.GetColValString("CFGITM_NAME"));
							tgctp05.SetColVal("OPERATE_TYPE", "11");
							tgctp05.SetColVal("NOW_ROW", GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn));
							tgctp05.SetColVal("ITEM_ENAME", "OPERATE_FUNCTION");

							if (subCfgitemName.Substring(subCfgitemName.GetLength() - 2, 1) == "B")
							{
								tgctp05.SetColVal("ITEM_CVALUE", "20");
							}
							else
							{
								tgctp05.SetColVal("ITEM_CVALUE", "01");
							}

							if (tgctp05.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

							tgctp05.SetColVal("ITEM_ENAME", "OPERATE_PAGE_ID");
							tgctp05.SetColVal("ITEM_CVALUE", "1");

							if (tgctp05.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

							//输入参数配置表数据新增
							tgctp06.SetColVal("ID", buttonId);
							tgctp06.SetColVal("FORM_NO", tgctp04.GetColValString("FORM_NO"));
							tgctp06.SetColVal("NOW_ROW", tgctp05.GetColValString("NOW_ROW"));
							tgctp06.SetColVal("PAGE_ID", "1");
							tgctp06.SetColVal("HANDLE_DIV", "0");
							tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
							tgctp06.SetColVal("PARA_TYPE", "01");
							tgctp06.SetColVal("CFGITM_NAME", mainCfgitemName);
							tgctp06.SetColVal("DATA_ORIGIN", "02");
							tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
							tgctp06.SetColVal("OPERATE_OBJECT", "2");
							tgctp06.SetColVal("OPERATE_MODE", " ");

							if (tgctp06.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}

							//输出参数配置表数据新增
							tgctp06.SetColVal("HANDLE_DIV", "1");
							tgctp06.SetColVal("CFGITM_NAME", subCfgitemName);
							tgctp06.SetColVal("DATA_ORIGIN", " ");

							if (subCfgitemName.Substring(subCfgitemName.GetLength() - 2, 1) == "B")
							{
								tgctp06.SetColVal("OBJECT_AREA", "03");
							}
							else
							{
								tgctp06.SetColVal("OBJECT_AREA", "02");
							}

							tgctp06.SetColVal("SHOW_FLAG", "0");

							if (tgctp06.Insert() < 0)
							{
								throw CApplicationException(-1, s.msg, s.svc_name);
							}
						}
					}
					else
					{
						sqlstr = "UPDATE tgctp05 SET operate_type = '" + operateType + "' WHERE id = " + buttonId.ToString();
						PrintLog("sqlstr", sqlstr);
						cmd.SetCommandText(sqlstr);
						cmd.ExecuteNonQuery();
						cmd.Close();
					}
					bcls_rec->Tables[blkName].Rows[j]["ID"] = buttonId;

					tgctp07.MergeFrom(bcls_rec->Tables[blkName].Rows[j]);
					tgctp07.SetColVal("ID", buttonId);
					tgctp07.SetColVal("SEQ_NO", (CDecimal)j + 1);
					tgctp07.SetColVal("FORM_NO", formNo);

					if (tgctp07.Insert() < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
			else if (blkName == "OPERATE_DO")
			{
				//PrintDataTable(bcls_rec->Tables[blkName]);

				CString formNo = bcls_rec->Tables[0].Rows[0]["FORM_NO"].ToString();
				PrintLog("formNo", formNo);

				CString buttonName = GetColValueC(bcls_rec->Tables[0], 0, "NAME");
				PrintLog("buttonName", buttonName);

				CDecimal buttonId = GetColValueD(bcls_rec->Tables[0], 0, "ID");
				PrintLog("buttonId", buttonId);

				if (buttonId > 0)
				{
					sqlstr = "DELETE FROM tgctp05 WHERE form_no = '" + formNo + "' AND id = " + buttonId.ToString();
					PrintLog("sqlstr", sqlstr);
					cmd.SetCommandText(sqlstr);
					cmd.ExecuteNonQuery();
					cmd.Close();
				}
				else if (buttonName.Trim() != "")
				{
					sqlstr = "DELETE FROM tgctp05 WHERE form_no = '" + formNo + "' AND name = '" + buttonName + "'";
					PrintLog("sqlstr", sqlstr);
					cmd.SetCommandText(sqlstr);
					cmd.ExecuteNonQuery();
					cmd.Close();
				}

				doFlag = f_gctp_getConfigItem(bcls_rec, bcls_ret, conn, blkName, "1");
				//PrintDataTable(bcls_ret->Tables[blkName]);

				iRowNo = 0;
				for (int j = 0; j < bcls_rec->Tables[blkName].Rows.get_Count(); j++)
				{
					PrintLog("j", j);

					buttonId = bcls_rec->Tables[blkName].Rows[j]["ID"].ToDecimal();
					PrintLog("buttonId", buttonId);

					if (buttonId == 0 && buttonName.Trim() != "")
					{
						buttonId = 1000000000 + GetSeqence("GCTP_BUTTON_SEQ_NO", conn);
					}

					sOperateRow = bcls_rec->Tables[blkName].Rows[j]["NOW_ROW"].ToString().Trim();
					PrintLog("sOperateRow", sOperateRow);

					if (sOperateRow == "")
					{
						sOperateRow = GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn);
					}

					CString cfgitmName = bcls_rec->Tables[blkName].Rows[j]["DATASET_NAME"].ToString();
					PrintLog("cfgitmName", cfgitmName);

					CString operateName = bcls_rec->Tables[blkName].Rows[j]["NAME"].ToString();
					PrintLog("operateName", operateName);

					if (operateName.Trim() == "" && buttonName.Trim() != "")
					{
						operateName = buttonName;
					}

					CString operateType = bcls_rec->Tables[blkName].Rows[j]["OPERATE_TYPE"].ToString();
					PrintLog("operateType", operateType);

					for (int k = 0; k < bcls_ret->Tables[blkName].Rows.get_Count(); k++)
					{
						CString colName = bcls_ret->Tables[blkName].Rows[k]["ITEM_ENAME"].ToString();
						CString colValue = bcls_rec->Tables[blkName].Rows[j][colName].ToString().Trim();

						if (colValue != "")
						{
							tgctp05.SetColVal("FORM_NO", formNo, iRowNo);
							tgctp05.SetColVal("ID", buttonId, iRowNo);
							tgctp05.SetColVal("NAME", operateName, iRowNo);
							tgctp05.SetColVal("SEQ_NO", (CDecimal)j + 1, iRowNo);
							tgctp05.SetColVal("DATASET_NAME", cfgitmName, iRowNo);
							tgctp05.SetColVal("OPERATE_TYPE", operateType, iRowNo);
							tgctp05.SetColVal("ITEM_ENAME", colName, iRowNo);
							tgctp05.SetColVal("NOW_ROW", sOperateRow, iRowNo);
							tgctp05.SetColVal("ITEM_CVALUE", colValue, iRowNo);
							iRowNo++;
						}
					}

					/*
					if (newFlag != "1" &&
						(!bcls_rec->Tables.Contains("OPERATE_IN") || bcls_rec->Tables["OPERATE_IN"].Rows.get_Count() == 0))
					{
						PrintLog("没有传入输入配置，自动生成");
						CString operateFunction = bcls_rec->Tables[blkName].Rows[j]["OPERATE_FUNCTION"].ToString();
						if (operateFunction == "01" || operateFunction == "02" || operateFunction == "03" ||
							operateFunction == "04" || operateFunction == "05")
						{
							tgctp06.SetFilterColVal("FORM_NO", formNo);
							tgctp06.SetFilterColVal("NOW_ROW", sOperateRow);

							if (tgctp06.QueryCount() == 0)
							{
								tgctp06.SetColVal("ID", buttonId);
								tgctp06.SetColVal("NAME", operateName);
								tgctp06.SetColVal("PAGE_ID", "0");
								tgctp06.SetColVal("HANDLE_DIV", "0");
								tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)1);
								tgctp06.SetColVal("CFGITM_NAME", cfgitmName);
								tgctp06.SetColVal("PARA_TYPE", "01");

								if (operateFunction == "01")
								{
									tgctp06.SetColVal("DATA_ORIGIN", "01");
									tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
								}
								else
								{
									tgctp06.SetColVal("DATA_ORIGIN", "02");
									tgctp06.SetColVal("OPERATE_OBJECT", "0");
									tgctp06.SetColVal("ITEM_MUST_FLAG", "1");

									if (operateFunction == "02")
									{
										tgctp06.SetColVal("OPERATE_MODE", "1");
									}
									else if (operateFunction == "05")
									{
										tgctp06.SetColVal("OPERATE_MODE", "3");
									}
									else
									{
										tgctp06.SetColVal("OPERATE_MODE", "0");
									}
								}

								if (tgctp06.Insert() < 0)
								{
									throw CApplicationException(-1, s.msg, s.svc_name);
								}

								if (operateFunction != "04")
								{
									tgctp06.SetColVal("HANDLE_DIV", "1");
									tgctp06.SetColVal("DATA_ORIGIN", " ");
									tgctp06.SetColVal("OPERATE_OBJECT", " ");
									tgctp06.SetColVal("OPERATE_MODE", " ");
									tgctp06.SetColVal("ITEM_MUST_FLAG", "0");
									tgctp06.SetColVal("OBJECT_AREA", "02");
									tgctp06.SetColVal("SHOW_FLAG", "0");

									if (tgctp06.Insert() < 0)
									{
										throw CApplicationException(-1, s.msg, s.svc_name);
									}
								}

							}
						}
					}
					*/
				}

				//新增数据
				if (tgctp05.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				//清空不存在now_row的tgctp06表数据
				sqlstr = "DELETE FROM tgctp06 WHERE form_no = '" + formNo + "' AND now_row NOT IN"
					" (SELECT now_row FROM tgctp05 WHERE form_no = '" + formNo + "')";
				PrintLog("sqlstr", sqlstr);
				cmd.SetCommandText(sqlstr);
				int delCount = cmd.ExecuteNonQuery();
				cmd.Close();
				PrintLog("delCount", delCount);
			}
			else if (blkName == "OPERATE")
			{
				tgctp06.CopyFrom(bcls_rec->Tables[blkName]);
				tgctp06.Print();
				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
				}
			}
			else if (blkName == "OPERATE_IN" || blkName == "OPERATE_OUT")
			{
				//PrintDataTable(bcls_rec->Tables[blkName]);

				tgctp06.CopyFrom(bcls_rec->Tables[blkName]);

				if (blkName == "OPERATE_IN")
				{
					tgctp06.SetColValAllRow("HANDLE_DIV", "0");
				}
				else
				{
					tgctp06.SetColValAllRow("HANDLE_DIV", "1");
				}

				formNo = bcls_rec->Tables[0].Rows[0]["FORM_NO"].ToString();
				PrintLog("formNo", formNo);

				CString buttonName = bcls_rec->Tables[0].Rows[0]["NAME"].ToString();
				PrintLog("buttonName", buttonName);

				CString buttonId = GetColValueC(bcls_rec->Tables[0], 0, "ID").Trim();
				PrintLog("buttonId", buttonId);

				CString nowRow = GetColValueC(bcls_rec->Tables[0], 0, "NOW_ROW").Trim();
				PrintLog("nowRow", nowRow);

				if (nowRow == "")
				{
					nowRow = sOperateRow;
				}

				if (buttonId != "" && nowRow != "")
				{
					sqlstr = "DELETE FROM tgctp06 WHERE form_no = '" + formNo + "' AND now_row = '" + nowRow + "'";
					if (blkName == "OPERATE_IN")
					{
						sqlstr += " AND handle_div = '0'";
					}
					else
					{
						sqlstr += " AND handle_div = '1'";
					}

					cmd.SetCommandText(sqlstr);
					cmd.ExecuteNonQuery();
					cmd.Close();
					PrintLog("sqlstr", sqlstr);
				}

				for (int j = 0; j < tgctp06.GetRowCount(); j++)
				{
					tgctp06.SetColVal("FORM_NO", formNo, j);
					tgctp06.SetColVal("ID", buttonId, j);
					tgctp06.SetColVal("PROC_SEQ_NO", (CDecimal)j + 1, j);
					tgctp06.Print("NOW_ROW", j);
					if (tgctp06.GetColValString("NOW_ROW", j).Trim() == "")
					{
						tgctp06.SetColVal("NOW_ROW", nowRow, j);
					}
				}

				tgctp06.Print();
				if (tgctp06.Insert() < 0)
				{
					throw CApplicationException(-1, s.msg, s.svc_name);
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

	return doFlag;
}