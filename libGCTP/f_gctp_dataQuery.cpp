/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      admin
Version:     1.0
Date:        2018-12-17 16:14:41
Description: 数据查询函数
**************************************************/

#include "CDynaTable2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigData(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn);

BM2_FUNCTION_IMPORT
int f_gctp_getTableItem(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, int table_type, CString table_name);

BM2_FUNCTION_EXPORT
int f_gctp_dataQuery(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDataTable dtMain;
	CDataTable dtMainItem;
	CDataTable dtCondtion;

	CString sTableType = "";
	CString sTableName = "";
	CString sDataSource = "";
	CString sDataSet = "";
	CString sInputCheckFlag = "";
	CString sJoinType = "";
	CString sJoinTableName1 = "";
	CString sJoinTableName2 = "";
	CString sJoinTableName3 = "";
	CString sJoinTableName4 = "";
	CString sTableSelectFlag = "";
	CString sTableSelectFlag1 = "";
	CString sTableSelectFlag2 = "";
	CString sTableSelectFlag3 = "";
	CString sMergeColumn = "";
	CString sQuerySql = "";
	CString sAddWhereSql = "";
	CString sAddSelectSql = "";
	CString strSelect = "";
	CString strFrom = "";
	CString strWhere = "";
	CString strOrderBy = "";
	
	int iRecordFrom = 0;
	int iPageSize = 0;
	int iTotalRecord = 0;

	CDbCommand cmd_inq(conn);

	try
	{
		if (bcls_rec->Tables.Contains("DS_MAIN") && bcls_rec->Tables.Contains("DS_ITEM"))
		{
			dtMain.Copy(bcls_rec->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_rec->Tables["DS_ITEM"]);
		}
		else if (bcls_rec->Tables[0].Rows.get_Count() > 0 && GetColValueC(bcls_rec->Tables[0], 0, "CFGITM_NAME").Trim() != "")
		{
			doFlag = f_gctp_getConfigData(bcls_rec, bcls_ret, conn);
			if (doFlag < 0)
			{
				throw CApplicationException(-1, s.msg, log.Location);
			}

			dtMain.Copy(bcls_ret->Tables["DS_MAIN"]);
			dtMainItem.Copy(bcls_ret->Tables["DS_ITEM"]);
		}
		else
		{
			strcpy(s.msg, "传入数据不完整");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		if (bcls_rec->Tables.Contains("OPERATE_DO") && bcls_rec->Tables["OPERATE_DO"].Rows.get_Count() > 0 &&
			GetColValueC(bcls_rec->Tables["OPERATE_DO"],0,"CUSTOM_TABLE_NAME").Trim() != "")
		{
			sTableType = "0";
			sTableName = GetColValueC(bcls_rec->Tables["OPERATE_DO"], 0, "CUSTOM_TABLE_NAME").Trim().ToUpper();
		}
		else
		{
			sTableType = dtMain.Rows[0]["TABLE_TYPE"].ToString();
			if (sTableType.Trim() == "")
			{
				strcpy(s.msg, "没有配置数据表类型");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			PrintLog("sTableType", sTableType);

			sTableName = dtMain.Rows[0]["TABLE_NAME"].ToString().Trim().ToUpper();
			if (sTableName == "")
			{
				strcpy(s.msg, "没有配置数据表名");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (sTableType == "3")
			{
				PrintLog("数据字典查询");
				return f_gctp_getTableItem(bcls_rec, bcls_ret, conn, 1, sTableName);
			}

			if (sTableType == "1")
			{
				//在线历史联合查询表，传入表首位为&
				sTableName = "&" + sTableName.Substring(1);
			}
			PrintLog("sTableName", sTableName);

			if (GetColValueC(bcls_rec->Tables[0], 0, "ONLINE_FLAG") == "1")
			{
				//历史表查询
				sTableName = "H" + sTableName.Substring(1);
			}
			PrintLog("sTableName", sTableName);

			sJoinTableName1 = GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_1").Trim().ToUpper();
			if (GetColValueC(dtMain, 0, "JOIN_TABLE_TYPE_1") == "1")
			{
				//在线历史联合查询表，传入表首位为&
				sJoinTableName1 = "&" + sJoinTableName1.Substring(1);
			}
			PrintLog("sJoinTableName1", sJoinTableName1);

			sJoinTableName2 = GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_2").Trim().ToUpper();
			if (GetColValueC(dtMain, 0, "JOIN_TABLE_TYPE_2") == "1")
			{
				//在线历史联合查询表，传入表首位为&
				sJoinTableName2 = "&" + sJoinTableName2.Substring(1);
			}
			PrintLog("sJoinTableName2", sJoinTableName2);

			sJoinType = GetColValueC(dtMain, 0, "JOIN_TYPE");
			PrintLog("sJoinType", sJoinType);

			sTableSelectFlag = GetColValueC(dtMain, 0, "TABLE_SELECT_FLAG");
			PrintLog("sTableSelectFlag", sTableSelectFlag);

			sTableSelectFlag1 = GetColValueC(dtMain, 0, "TABLE_SELECT_FLAG_1");
			PrintLog("sTableSelectFlag1", sTableSelectFlag);

			sTableSelectFlag2 = GetColValueC(dtMain, 0, "TABLE_SELECT_FLAG_2");
			PrintLog("sTableSelectFlag2", sTableSelectFlag);

			sTableSelectFlag3 = GetColValueC(dtMain, 0, "TABLE_SELECT_FLAG_3");
			PrintLog("sTableSelectFlag3", sTableSelectFlag);

			sQuerySql = GetColValueC(dtMain, 0, "CUSTOM_QUERY_SQL");
			PrintLog("sQuerySql", sQuerySql);

			PrintLog("ADD_STATEMENT_CUSTOM", GetColValueC(dtMain, 0, "ADD_STATEMENT_CUSTOM"));
			PrintLog("ADD_STATEMENT", GetColValueC(dtMain, 0, "ADD_STATEMENT"));

			if (GetColValueC(dtMain, 0, "ADD_STATEMENT_CUSTOM").Trim() != "")
			{
				sAddWhereSql = GetColValueC(dtMain, 0, "ADD_STATEMENT_CUSTOM");
			}
			else
			{
				sAddWhereSql = GetColValueC(dtMain, 0, "ADD_STATEMENT");
			}

			if (sTableName == "HMMCP01")
			{
				sAddWhereSql += "event_id = 'SM01'";
			}

			PrintLog("sAddWhereSql", sAddWhereSql);

			sAddSelectSql = GetColValueC(dtMain, 0, "ADD_SELECT");
			PrintLog("sAddSelectSql", sAddSelectSql);
		}

		if (bcls_rec->Tables.Contains("OPERATE_DO") && bcls_rec->Tables["OPERATE_DO"].Rows.get_Count() > 0)
		{
			sDataSource = "DEFAULT_ENABLE_STATE_" + bcls_rec->Tables["OPERATE_DO"].Rows[0]["OPERATE_BUTTON"].ToString();
			sDataSet = "ENABLE_CTRL_STR_" + bcls_rec->Tables["OPERATE_DO"].Rows[0]["OPERATE_BUTTON"].ToString();
			sInputCheckFlag = bcls_rec->Tables["OPERATE_DO"].Rows[0]["INPUT_CHECK_FLAG"].ToString();
			PrintLog("sInputCheckFlag", sInputCheckFlag);
		}
		else
		{
			sDataSource = "DEFAULT_ENABLE_STATE_2";
			sDataSet = "ENABLE_CTRL_STR_2";
		}

		if (bcls_rec->Tables.Contains("PAGEINFO"))
		{
			iRecordFrom = GetColValueD(bcls_rec->Tables["PAGEINFO"], 0, "NOW_RECORD").ToInt32();
			PrintLog("iRecordFrom", iRecordFrom);

			iPageSize = GetColValueD(bcls_rec->Tables["PAGEINFO"], 0, "PAGE_SIZE").ToInt32();
			PrintLog("iPageSize", iPageSize);
		}

		//定义数据表
		CDynaTable2 dynaTable(sTableName, conn);

		//设置连接表名
		if (sJoinType == "U")
		{
			if (sJoinTableName1 != "")
			{
				dynaTable.AddUnionTable(sJoinTableName1);
			}

			if (sJoinTableName2 != "")
			{
				dynaTable.AddUnionTable(sJoinTableName2);
			}

			if (GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_3").Trim() != "")
			{
				sJoinTableName3 = GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_3").Trim().ToUpper();
				PrintLog("sJoinTableName3", sJoinTableName3);
				dynaTable.AddUnionTable(sJoinTableName3);
			}

			if (GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_4").Trim() != "")
			{
				sJoinTableName4 = GetColValueC(dtMain, 0, "JOIN_TABLE_NAME_4").Trim().ToUpper();
				PrintLog("sJoinTableName4", sJoinTableName4);
				dynaTable.AddUnionTable(sJoinTableName4);
			}

			dynaTable.SetUnionTable();
		}
		else if (sJoinTableName1.Trim() != "")
		{
			dynaTable.SetJoinTable(sJoinTableName1, sJoinTableName2, sJoinType);
		}

		//PrintDataTableColumns(bcls_rec->Tables[0]);

		if (sTableType == "2")
		{

		}
		else
		{
			if (sQuerySql.Trim() == "")
			{
				for (int i = 0; i < dtMainItem.Rows.get_Count(); i++)
				{
					CString colName = dtMainItem.Rows[i]["COLUMN_NAME"].ToString();

					if (GetColValueC(dtMainItem, i, sDataSource) == "3")
					{
						if (bcls_rec->Tables[0].Rows.get_Count() == 0)
						{
							bcls_rec->Tables[0].Rows.Add();
						}

						if (dtMainItem.Rows[i]["DATA_TYPE"].ToString() == "N")
						{
							if (dtMainItem.Rows[i][sDataSet].ToString().Trim() == "")
							{
								AddColValue(bcls_rec->Tables[0], colName, dtMainItem.Rows[i][sDataSet].ToDecimal());
							}
						}
						else
						{
							if (dtMainItem.Rows[i][sDataSet].ToString().Trim() != "")
							{
								AddColValue(bcls_rec->Tables[0], colName, dtMainItem.Rows[i][sDataSet].ToString());
							}
						}
					}
					else if (GetColValueC(dtMainItem, i, sDataSource) == "8")
					{
						if (bcls_rec->Tables[0].Rows.get_Count() == 0)
						{
							bcls_rec->Tables[0].Rows.Add();
						}

						AddColValue(bcls_rec->Tables[0], colName, (CString)s.userid);
					}
					else if (GetColValueC(dtMainItem, i, sDataSource) == "C")
					{
						if (bcls_rec->Tables[0].Rows.get_Count() == 0)
						{
							bcls_rec->Tables[0].Rows.Add();
						}

						AddColValue(bcls_rec->Tables[0], colName, (CString)s.fore_machine);
					}

					if (sTableSelectFlag == "1")
					{
						if (dtMainItem.Rows[i]["TABLE_NAME"].ToString().Substring(1) == sTableName.Substring(1) &&
							dtMainItem.Rows[i]["DEFAULT_SHOW_STATE_2"].ToString() == "1")
						{
							dynaTable.AddSelectColName(colName);
						}
					}

					if (sJoinType == "I" || sJoinType == "L" || sJoinType == "R" || sJoinType == "F")
					{
						//PrintLog("添加连接字段");

						if (sJoinTableName1 != "" && dtMainItem.Rows[i]["TABLE_NAME"].ToString().Substring(1) == sJoinTableName1.Substring(1))
						{
							PrintLog("sJoinTableName1", sJoinTableName1);

							if (sTableSelectFlag1 != "0" && dtMainItem.Rows[i]["DEFAULT_SHOW_STATE_2"].ToString() == "1")
							{
								dynaTable.AddJoinSelectColName1(colName);
							}

							if (dtMainItem.Rows[i]["FOREIGN_KEY_SEQ"].ToDecimal() > 0)
							{
								PrintLog("FOREIGN_KEY_SEQ colName", colName);

								CString strJoin = "t1." + colName + " = ";
								PrintLog("1.strJoin", strJoin);

								int joinFlag = 0;
								for (int j = 0; j < dtMainItem.Rows.get_Count(); j++)
								{
									if (dtMainItem.Rows[j]["TABLE_NAME"].ToString().Substring(1) == sTableName.Substring(1) &&
										dtMainItem.Rows[j]["FOREIGN_KEY_SEQ"].ToDecimal() == dtMainItem.Rows[i]["FOREIGN_KEY_SEQ"].ToDecimal())
									{
										joinFlag = 1;
										strJoin += "t." + dtMainItem.Rows[j]["COLUMN_NAME"].ToString();
										PrintLog("2.strJoin", strJoin);

										break;
									}
								}

								if (joinFlag == 0)
								{
									strJoin += "t." + colName;
								}

								dynaTable.AddJoinColName1(strJoin);
							}
							else if (dtMainItem.Rows[i]["KEYWORD_FLAG"].ToString() == "1")
							{
								CString strJoin = "t1." + colName + " = t." + colName;
								dynaTable.AddJoinColName1(strJoin);

								for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
								{
									if (bcls_rec->Tables[0].Columns[j].get_ColumnName().Find(colName) >= 0)
									{
										bcls_rec->Tables[0].Columns[j].set_Caption(sJoinTableName1);
									}
								}
							}
						}					
						else if (sJoinTableName2 != "" && dtMainItem.Rows[i]["TABLE_NAME"].ToString().Substring(1) == sJoinTableName2.Substring(1))
						{
							PrintLog("sJoinTableName2", sJoinTableName2);

							if (sTableSelectFlag2 != "0" && dtMainItem.Rows[i]["DEFAULT_SHOW_STATE_2"].ToString() == "1")
							{
								dynaTable.AddJoinSelectColName2(colName);
							}

							if (dtMainItem.Rows[i]["FOREIGN_KEY_SEQ"].ToDecimal() > 0)
							{
								CString strJoin = "t2." + colName + " = ";
								int joinFlag = 0;

								for (int j = 0; j < dtMainItem.Rows.get_Count(); j++)
								{
									if (dtMainItem.Rows[j]["TABLE_NAME"].ToString().Substring(1) == sTableName.Substring(1) &&
										dtMainItem.Rows[j]["FOREIGN_KEY_SEQ"].ToDecimal() == dtMainItem.Rows[i]["FOREIGN_KEY_SEQ"].ToDecimal())
									{
										joinFlag = 1;
										strJoin += "t." + dtMainItem.Rows[j]["COLUMN_NAME"].ToString();
										PrintLog("2.strJoin", strJoin);

										break;
									}
								}

								if (joinFlag == 0)
								{
									strJoin += "t." + colName;
								}

								dynaTable.AddJoinColName2(strJoin);
							}
							else if (dtMainItem.Rows[i]["KEYWORD_FLAG"].ToString() == "1")
							{
								CString strJoin = "t2." + colName + " = t." + colName;
								dynaTable.AddJoinColName1(strJoin);

								for (int j = 0; j < bcls_rec->Tables[0].Columns.get_Count(); j++)
								{
									if (bcls_rec->Tables[0].Columns[j].get_ColumnName().Find(colName) >= 0)
									{
										bcls_rec->Tables[0].Columns[j].set_Caption(sJoinTableName2);
									}
								}
							}
						}
					}

					if (dtMainItem.Rows[i]["ORDER_MARK"].ToString() == "A")
					{
						dynaTable.AddOrderByAscColName(colName);
					}
					else if (dtMainItem.Rows[i]["ORDER_MARK"].ToString() == "D")
					{
						dynaTable.AddOrderByDescColName(colName);
					}

				}

				strSelect = dynaTable.BuildSelectString();

				if (sAddSelectSql.Trim() != "")
				{
					strSelect += "," + sAddSelectSql;
				}
				//PrintLog("strSelect", strSelect);

				strFrom = dynaTable.BuildFromString();
				//PrintLog("strFrom", strFrom);

				if (bcls_rec->Tables[0].Rows.get_Count() > 0)
				{
					strWhere = dynaTable.BuildWhereString(bcls_rec->Tables[0].Rows[0]);
				}

				PrintLog("strWhere", strWhere);
				PrintLog("sInputCheckFlag", sInputCheckFlag);

				if (sInputCheckFlag == "1" && strWhere.Trim() == "")
				{
					strcpy(s.msg, "请输入查询条件");
					throw CApplicationException(-1, s.msg, s.svc_name);
				}

				if (sAddWhereSql.Trim() != "")
				{
					if (strWhere.Trim() == "")
					{
						strWhere = sAddWhereSql;
					}
					else
					{
						strWhere += " AND " + sAddWhereSql;
					}
				}
				//PrintLog("strWhere", strWhere);

				strOrderBy = dynaTable.BuildOrderByString();
				sQuerySql = strSelect + strFrom;

				if (strWhere.Trim() != "")
				{
					sQuerySql += " WHERE " + strWhere.Trim();
				}

				if (strOrderBy.Trim() != "")
				{
					sQuerySql += strOrderBy;
				}
			}
			else
			{
				if (bcls_rec->Tables[0].Rows.get_Count() > 0)
				{
					strWhere = dynaTable.BuildWhereString(bcls_rec->Tables[0].Rows[0]);
					if (strWhere.Trim() != "")
					{
						if (sQuerySql.ToUpper().Find("WHERE") > 0)
						{
							sQuerySql += " AND " + strWhere.Trim();
						}
						else
						{
							sQuerySql += " WHERE " + strWhere.Trim();
						}
					}
					else if (sInputCheckFlag == "1")
					{
						strcpy(s.msg, "请输入查询条件");
						throw CApplicationException(-1, s.msg, s.svc_name);
					}

					strOrderBy = dynaTable.BuildOrderByString();
					if (strOrderBy.Trim() != "")
					{
						sQuerySql += strOrderBy;
					}
				}
			}

			PrintLog("sQuerySql", sQuerySql);
			cmd_inq.SetCommandText(sQuerySql);
			if (iPageSize > 0)
			{
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0], iRecordFrom, iPageSize);
			}
			else
			{
				cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);
			}
			cmd_inq.Close();

			if (iPageSize > 0)
			{
				iTotalRecord = dynaTable.QueryCount(strWhere);
			}
			else
			{
				iTotalRecord = bcls_ret->Tables[0].Rows.get_Count();
			}

			PrintLog("iTotalRecord", iTotalRecord);
			PrintLog("总行数", dynaTable.GetRowCount());
			if (iTotalRecord < 0)
			{
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (!bcls_ret->Tables[0].Columns.Contains("NOW_ROW"))
			{
				bcls_ret->Tables[0].Columns.Add(DT_STRING, "NOW_ROW");
			}

			for (int i = 0; i < bcls_ret->Tables[0].Rows.get_Count(); i++)
			{
				bcls_ret->Tables[0].Rows[i]["NOW_ROW"] = CDecimal(i).ToString();

				//for (int j = 0; j < bcls_ret->Tables[0].Columns.get_Count(); j++)
				//{
				//	if (bcls_ret->Tables[0].Columns[j].get_DataType() == DT_DECIMAL)
				//	{
				//		//PrintLog("get_ColumnName", bcls_ret->Tables[0].Columns[j].get_ColumnName());
				//		for (int k = 0; k < dtMainItem.Rows.get_Count(); k++)
				//		{
				//			if (dtMainItem.Rows[k]["COLUMN_NAME"].ToString() == bcls_ret->Tables[0].Columns[j].get_ColumnName())
				//			{
				//				double transFactor = dtMainItem.Rows[k]["TRANS_FACTOR"].ToDecimal().ToDouble();
				//				//PrintLog("transFactor", transFactor);
				//				if (transFactor != 0)
				//				{
				//					bcls_ret->Tables[0].Rows[i][j] = bcls_ret->Tables[0].Rows[i][j].ToDecimal() * pow(10, transFactor);
				//				}

				//				break;
				//			}
				//		}
				//	}
				//	else if (bcls_ret->Tables[0].Columns[j].get_DataType() == DT_STRING &&
				//		bcls_ret->Tables[0].Rows[i][j].ToString().Trim() == "")
				//	{
				//		bcls_ret->Tables[0].Rows[i][j] = "";
				//	}
				//}
			}

			for (int i = 0; i < dtMainItem.Rows.get_Count(); i++)
			{
				if (dtMainItem.Rows[i]["TRANS_FACTOR"].ToDecimal().ToDouble() != 0)
				{
					double transFactor = dtMainItem.Rows[i]["TRANS_FACTOR"].ToDecimal().ToDouble();
					CString colName = dtMainItem.Rows[i]["COLUMN_NAME"].ToString();
					for (int j = 0; j < bcls_ret->Tables[0].Rows.get_Count(); j++)
					{
						bcls_ret->Tables[0].Rows[j][colName] = bcls_ret->Tables[0].Rows[j][colName].ToDecimal() * pow(10, transFactor);
					}
				}
			}
		}

		if (iPageSize > 0)
		{
			//返回分页信息
			bcls_ret->Tables.Add("PAGEINFO");	//增加块
			bcls_ret->Tables["PAGEINFO"].Columns.Add(DT_DECIMAL, "TOTAL_RECORD");	//总记录数
			bcls_ret->Tables["PAGEINFO"].Rows.Add();
			bcls_ret->Tables["PAGEINFO"].Rows[0][0] = iTotalRecord;
		}

		//PrintDataTable(bcls_ret->Tables[0]);
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