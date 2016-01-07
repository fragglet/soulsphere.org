<%@page import="java.io.*" %>
<%@page import="java.util.*" %>

<%!

/*

  Copyright(C) 2005 Simon Howard

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
  SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
  IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.


  JSP Session Browser: displays the contents of your JSP session and
  allows manipulation.
*/

void listObjects(HttpSession session, JspWriter out)
	throws Exception
{
    int elements = 0;

    out.print("<table border=\"2\">");
    out.print("<tr><td><b>Name</b></td><td><b>Class</b></td></tr>");

    Enumeration e = session.getAttributeNames();

    while (e.hasMoreElements())
    {
        String name = (String) e.nextElement();

        out.print("<tr>");
        out.print("<td><a href=\"?name=" + name + "\">" + name + "</a></td>");
	out.print("<td>" + session.getAttribute(name).getClass().getName() + "</td>");
        out.print("<td><a href=\"?action=delete&name=" + name + "\">x</a></td>");
	out.print("</tr>");
        ++elements;
    }

    out.print("</table>");
    out.print("<br>");
    if (elements > 0)
        out.println("<a href=\"?action=deleteall&name=dummy\">delete all</a>");
    else
        out.println("(session is empty)");
}

void showObjectDetail(HttpSession session, JspWriter out, String name)
	throws Exception
{
    out.print("<h3> Object: " + name + "</h3>");
    out.print("<a href=\"?\">(back)</a>");
    out.print("<br><br>");
    out.print("<table border=\"2\"><tr><td>");
    out.print("<pre>" + session.getAttribute(name) + "</pre>");
    out.print("</td></tr></table>");
}

void deleteAllObjects(HttpSession session)
	throws Exception
{
    Enumeration e = session.getAttributeNames();

    while (e.hasMoreElements())
    {
        String name = (String) e.nextElement();

        session.removeAttribute(name);
    }
}
%>

<h1> The amazing JSP session browser </h1>

<%

String actionType = (String) request.getParameter("action");
String detailName = (String) request.getParameter("name");

if (detailName != null)
{
    if (actionType != null) {
        if (actionType.equals("delete")) {
            session.removeAttribute(detailName);
            out.println(detailName + " removed.");
            listObjects(session, out);
        } else if (actionType.equals("deleteall")) {
            deleteAllObjects(session);
            listObjects(session, out);
        } else if (actionType.equals("invalidate")) {
            session.invalidate();
            out.println("Session invalidated.");
            return;
        }
    } else {
        showObjectDetail(session, out, detailName);
    }
}
else
{
    listObjects(session, out);
}

%>

<hr>

Session created <%= new Date(session.getCreationTime()) %>.
<a href="?action=invalidate&name=x">invalidate session</a>

