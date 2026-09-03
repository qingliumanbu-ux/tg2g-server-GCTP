/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:        2022-07-08 16:11:53
Description: 获取配置竖表数据
**************************************************/

#include "CUtils2.h"

BM2_FUNCTION_IMPORT
int f_gctp_getConfigItem(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmGrpName, CString custFlag);

BM2_FUNCTION_EXPORT
int f_gctp_getCfgDataVt(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName, CString cfgitmGrpName)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDataTable dtQuery;
	EIClass bcls_rec_cfg;
	EIClass bcls_ret_cfg;

	//数据库操作类定义
	CDbCommand cmd_inq(conn);

	try
	{
		doFlag = f_gctp_getConfigItem(&bcls_rec_cfg, &bcls_ret_cfg, conn, cfgitmGrpName, "");
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//查询配置表主项数据
		sqlstr = "SELECT * FROM tgctp01 WHERE cfgitm_name = '" + cfgitmName + "' AND cfgitm_grp_name = '" + cfgitmGrpName + "' ORDER BY now_row";
		PrintLog("sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(dtQuery);
		cmd_inq.Close();

		//PrintDataTable(dtQuery);
		if (dtQuery.Rows.get_Count() > 0)
		{
			if (!bcls_ret->Tables.Contains(cfgitmGrpName))
			{
				bcls_ret->Tables.Add(cfgitmGrpName);
			}

			bcls_ret->Tables[cfgitmGrpName].Columns.Add(DT_STRING, "CFGITM_NAME");

			CDataTable dtColumn = bcls_ret_cfg.Tables[cfgitmGrpName];

			for (int i = 0; i < dtQuery.Rows.get_Count(); i++)
			{
				if (i == 0 || dtQuery.Rows[i]["NOW_ROW"].ToString() != dtQuery.Rows[i - 1]["NOW_ROW"].ToString())
				{
					bcls_ret->Tables[cfgitmGrpName].Rows.Add();
				}

				CDataRow& drRet = bcls_ret->Tables[cfgitmGrpName].Rows[bcls_ret->Tables[cfgitmGrpName].Rows.get_Count() - 1];
				drRet["CFGITM_NAME"] = dtQuery.Rows[i]["CFGITM_NAME"];

				for (int j = 0; j < dtColumn.Rows.get_Count(); j++)
				{
					CString colName = dtColumn.Rows[j]["ITEM_ENAME"].ToString();
					CString colCaption = dtColumn.Rows[j]["ITEM_CNAME"].ToString();
					CString colDataType = dtColumn.Rows[j]["DATA_TYPE"].ToString();

					if (!bcls_ret->Tables[cfgitmGrpName].Columns.Contains(colName))
					{
						if (colDataType == "C" || colDataType == "T")
						{
							bcls_ret->Tables[cfgitmGrpName].Columns.Add(DT_STRING, colName);
						}
						else
						{
							bcls_ret->Tables[cfgitmGrpName].Columns.Add(DT_DECIMAL, colName);
						}

						bcls_ret->Tables[cfgitmGrpName].Columns[colName].set_Caption(colCaption);
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

				for (int j = 0; j < bcls_ret->Tables[cfgitmGrpName].Rows.get_Count(); j++)
				{
					if (colDataType == "N" || colDataType == "B")
					{
						if (!bcls_ret->Tables[cfgitmGrpName].Columns.Contains(colName) ||
							bcls_ret->Tables[cfgitmGrpName].Rows[j][colName].ToString().Trim() == "")
						{
							if (colDefaultValue == "")
							{
								AddColValue(bcls_ret->Tables[cfgitmGrpName], j, colName, (CDecimal)0);
							}
							else
							{
								AddColValue(bcls_ret->Tables[cfgitmGrpName], j, colName, dtColumn.Rows[i]["DEFAULT_VALUE"].ToDecimal());
							}
						}
					}
					else if (!bcls_ret->Tables[cfgitmGrpName].Columns.Contains(colName))
					{
						AddColValue(bcls_ret->Tables[cfgitmGrpName], j, colName, dtColumn.Rows[i]["DEFAULT_VALUE"].ToString());
					}
				}
			}

			if (cfgitmGrpName == "CONTROL_MAIN" &&
				bcls_ret->Tables[cfgitmGrpName].Rows[0]["ADD_CONTROL_METHOD"].ToString() == "2" &&
				bcls_ret->Tables[cfgitmGrpName].Rows[0]["ED54_FUNCTION_ID"].ToString().Trim() != "")
			{
				CString funcId = bcls_ret->Tables[cfgitmGrpName].Rows[0]["ED54_FUNCTION_ID"].ToString().Trim();
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

				AddColValue(bcls_ret->Tables[cfgitmGrpName], 0, "ROW_NUM", rowCount);
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


