/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2018
Author:      178773
Version:     1.0
Date:        2018-09-19 14:06:36
Description: 查询按钮配置数据
**************************************************/

#include "CUtils2.h"

BM2_FUNCTION_EXPORT
int f_gctp_buttonConfig(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn, CString formNo)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	CDataTable dtQuery;
	CDataTable dtColumn;
	CDbCommand cmd_inq(conn);

	try
	{
		PrintLog("LOAD_FLAG", GetColValueC(bcls_rec->Tables[0], 0, "LOAD_FLAG"));

		//查询事件配置数据
		bcls_ret->Tables.Add("CUSTOM_EVENT");
		sqlstr = "SELECT * FROM tgctp07 WHERE form_no = '" + formNo + "' ORDER BY seq_no";
		PrintLog("sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(bcls_ret->Tables["CUSTOM_EVENT"]);
		cmd_inq.Close();

		//查询配置表主项数据
		sqlstr = "SELECT * FROM tgctp05 WHERE form_no = '" + formNo + "' ORDER BY seq_no,now_row";
		PrintLog("sqlstr", sqlstr);
		cmd_inq.SetCommandText(sqlstr);
		cmd_inq.ExecuteQuery(dtQuery);
		cmd_inq.Close();

		if (dtQuery.Rows.get_Count() > 0)
		{
			bcls_ret->Tables.Add("OPERATE_DO");

			sqlstr = "SELECT * FROM tgctp00 WHERE cfgitm_grp_name = 'OPERATE_DO' ORDER BY seq_no";
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(dtColumn);
			cmd_inq.Close();
			
			for (int i = 0; i < dtQuery.Rows.get_Count(); i++)
			{
				if (i == 0 || dtQuery.Rows[i]["NOW_ROW"].ToString() != dtQuery.Rows[i - 1]["NOW_ROW"].ToString())
				{
					bcls_ret->Tables["OPERATE_DO"].Rows.Add();
				}

				CDataRow& drRet = bcls_ret->Tables["OPERATE_DO"].Rows[bcls_ret->Tables["OPERATE_DO"].Rows.get_Count() - 1];
				for (int j = 0; j < dtColumn.Rows.get_Count(); j++)
				{
					CString colName = dtColumn.Rows[j]["ITEM_ENAME"].ToString();
					CString colCaption = dtColumn.Rows[j]["ITEM_CNAME"].ToString();
					CString colDataType = dtColumn.Rows[j]["DATA_TYPE"].ToString();
					CString custFlag = dtColumn.Rows[j]["CUST_FLAG"].ToString();

					if (!bcls_ret->Tables["OPERATE_DO"].Columns.Contains(colName))
					{
						if (colDataType == "C" || colDataType == "T")
						{
							bcls_ret->Tables["OPERATE_DO"].Columns.Add(DT_STRING, colName);
						}
						else
						{
							bcls_ret->Tables["OPERATE_DO"].Columns.Add(DT_DECIMAL, colName);
						}

						bcls_ret->Tables["OPERATE_DO"].Columns[colName].set_Caption(colCaption);
					}

					if (custFlag == "1")
					{
						//PrintLog("竖表列", colName);
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
						}
					}
					else if (dtQuery.Columns.Contains(colName))
					{
						//PrintLog("横表列", colName);
						drRet[colName] = dtQuery.Rows[i][colName];
						//PrintLog("colValue", dtQuery.Rows[i][colName].ToString());
					}
					else
					{
						PrintLog("问题列", colName);
					}
				}
			}

			//PrintDataTable(bcls_ret->Tables["OPERATE_DO"]);

			bcls_ret->Tables.Add("OPERATE");
			sqlstr = "SELECT * FROM tgctp06 WHERE form_no = '" + formNo + "' ORDER BY handle_div,id,page_id,proc_seq_no";
			PrintLog("sqlstr", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteQuery(bcls_ret->Tables["OPERATE"]);
			cmd_inq.Close();			
		}

		//PrintDataTable(bcls_ret->Tables["CALL_BUTTON"]);
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


