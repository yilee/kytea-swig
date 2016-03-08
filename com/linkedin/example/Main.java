package com.linkedin.example;

import com.linkedin.kytea.Mykytea;
import com.linkedin.kytea.StringVector;

import java.io.File;


/**
 * Created by kshi on 3/7/16.
 */

public class Main {

    static {
        try {
            System.err.println("Now Loading the lib ...");
            File f = new File("your_path/_LiKytea.dylib");
            System.load(f.getAbsolutePath());
            System.err.println("Native Segmentation Library Loading is done!");
        } catch (Exception e) {
            System.err.println("Native code library failed to load.\n" + e);
            System.exit(1);
        }
    }

    public static void main(String[] args) {
        Mykytea myKytea = new Mykytea("--model " + "your_path/model.bin");
        String s = "北京领英中国赤兔公司中华人民大团结万岁! 全球最大的中文搜索引擎、致力于让网民更便捷地获取信息，找到所求。百度超过千亿的中文网页数据库，可以瞬间找到相关的搜索结果。";
        String result = myKytea.getTagsToString(s);
        System.out.println(result);
        StringVector sv = myKytea.getWS(s);
        for (int i = 0; i < sv.size(); i++) {
            System.out.print(sv.get(i)+" ");
        }
    }
}
