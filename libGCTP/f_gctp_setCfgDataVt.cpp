/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2022
Author:      admin
Version:     1.0
Date:        2022-07-12 16:28:56
Description: 保存竖表配置数据
**************************************************/

#include "CDynaTable2.h"

BM2_FUNCTION_EXPORT
int f_gctp_setCfgDataVt(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn, CString cfgitmName, CString cfgitmGrpName)
{
	CTracer log(__FUNCTION__);

	//程序内部变量
	int doFlag = 0;
	CString sqlstr = " ";

	//应用变量
	

	//数据库操作类定义
	CDbCommand cmd(conn);

	//动态数据表定义
	CDynaTable2 tgctp01("TGCTP01", conn);

	try
	{
		if (cfgitmName.Trim() != "" && cfgitmGrpName.Trim() != "" && bcls_rec->Tables.Contains(cfgitmGrpName) && bcls_rec->Tables[cfgitmGrpName].Columns.get_Count() > 0)
		{
			sqlstr = "DELETE FROM tgctp01 WHERE cfgitm_name = '" + cfgitmName + "' AND cfgitm_grp_name = '" + cfgitmGrpName + "'";
			cmd.SetCommandText(sqlstr);
			cmd.ExecuteNonQuery();
			cmd.Close();

			for (int i = 0; i < bcls_rec->Tables[cfgitmGrpName].Rows.get_Count(); i++)
			{
				if (GetColValueC(bcls_rec->Tables[cfgitmGrpName], i, "CFGITM_NAME").Trim() != "" &&
					GetColValueC(bcls_rec->Tables[cfgitmGrpName], i, "CFGITM_NAME") != cfgitmName)
				{
					continue;
				}

				CString sRowId = GetTrackSeqNo("GCTP_VTABLE_ROWID", 20, conn);

				for (int j = 0; j < bcls_rec->Tables[cfgitmGrpName].Columns.get_Count(); j++)
				{
					CString colName = bcls_rec->Tables[cfgitmGrpName].Columns[j].get_ColumnName();
					CString colValue = bcls_rec->Tables[cfgitmGrpName].Rows[i][colName].ToString();

					if (colName == "CFGITM_NAME")
					{
						continue;
					}

					if (bcls_rec->Tables[cfgitmGrpName].Columns[j].get_DataType() == DT_STRING)
					{
						//colValue = colValue.Replace("'", "''");
						if (colName == "ADD_STATEMENT")
						{
							PrintLog("ADD_STATEMENT", colValue);
						}
					}

					tgctp01.SetColVal("CFGITM_NAME", cfgitmName);
					tgctp01.SetColVal("CFGITM_GRP_NAME", cfgitmGrpName);
					tgctp01.SetColVal("ITEM_ENAME", colName);
					tgctp01.SetColVal("NOW_ROW", sRowId);
					tgctp01.SetColVal("ITEM_CVALUE", colValue);
					
					if (tgctp01.Insert() < 0)
					{
						throw CApplicationException(-1, s.msg, s.svc_name);
					}
				}
			}
		}
		else
		{
			PrintLog("传入数据不完整");

			//strcpy(s.msg, "传入数据不完整");
			//throw CApplicationException(-1, s.msg, s.svc_name);
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


