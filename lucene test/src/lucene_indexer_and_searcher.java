import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.file.Paths;
import java.util.Date;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.TextField;
import org.apache.lucene.index.DirectoryReader;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.index.IndexWriterConfig.OpenMode;
import org.apache.lucene.queryparser.classic.QueryParser;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;

import com.google.gson.JsonElement;
import com.google.gson.JsonObject;
import com.google.gson.JsonParser;

public class lucene_indexer_and_searcher
{
	public static void main(String[] args)
	{
		new lucene_indexer_and_searcher();
	}
	
	lucene_indexer_and_searcher()
	{
		//new searcher("i like playing pokemon");
		new indexer();
	}
	
	private class searcher
	{
		private searcher(String input_text)
		{
			try
			{
				int results_no = 100;
				
				IndexReader reader = DirectoryReader.open(FSDirectory.open(Paths.get(definitions.DEF_base_work_directory+definitions.DEF_data_file_merged_directory)));
				
				Analyzer analyzer = new StandardAnalyzer();
				
				Query query = new QueryParser("body", analyzer).parse(input_text);
				
			    IndexSearcher searcher = new IndexSearcher(reader);
			    
				TopDocs results = searcher.search(query, results_no);
				ScoreDoc[] hits = results.scoreDocs;
				
				System.out.println("Results: "+hits.length);
				for(int i = 0; i < Math.min(hits.length, results_no); i++)
				{
					Document doc = searcher.doc(hits[i].doc);
					
					String[] item_location = doc.get("location").split("-");
					String comment = get_comment_from_location(Integer.parseInt(item_location[0]), Long.parseLong(item_location[1]));
					
					System.out.println("Score: "+hits[i].score+" | Data: [[["+comment+"]]]");
				}
				
			    reader.close();
			}
			catch (Exception e)
			{
				e.printStackTrace();
			}
		}
		
		private String get_comment_from_location(int document_id, long location_id) throws IOException
		{
			String file = definitions.DEF_base_work_directory+definitions.data_file_folder_list[document_id]+"\\"+definitions.data_file_list[document_id];
			return extract_json_body(read_file(file, location_id));
			
		}
		
		private String read_file(String filePath, long position) throws IOException
		{
			RandomAccessFile file = new RandomAccessFile(filePath, "r");
			file.seek(position);
			String comment = file.readLine();
			file.close();
			
			return comment;
		}
		
		private String extract_json_body(String json_line)
		{
		    JsonElement jelement = new JsonParser().parse(json_line);
		    JsonObject  jobject = jelement.getAsJsonObject();
		    return jobject.get("body").getAsString();
		}
	}
	
	private class indexer
	{
		private indexer()
		{
		    Date start = new Date();
			System.out.println("Indexing started: "+start);
			try
			{
				Directory dir = FSDirectory.open(Paths.get(definitions.DEF_base_work_directory+definitions.DEF_data_file_merged_directory));
				Analyzer analyzer = new StandardAnalyzer();
				IndexWriterConfig iwc = new IndexWriterConfig(analyzer);
				
				iwc.setOpenMode(OpenMode.CREATE);
				iwc.setRAMBufferSizeMB(4096.0);//4GB memory
				
				IndexWriter writer = new IndexWriter(dir, iwc);
				
				parse_all_files(writer, start);
				
				writer.close();
				System.out.println("Total Time: "+(((new Date()).getTime() - start.getTime()))/1000.0+" total seconds");
			}
			catch (Exception e)
			{
				System.out.println(" caught a " + e.getClass() + "\n with message: " + e.getMessage());
			}
		}
		
		private void parse_all_files(IndexWriter writer, Date start)//, int current_thread, int total_threads)
		{
			for(int i = 0; i < definitions.data_file_folder_list.length; i++)
			{
				parse_entire_file(writer, definitions.DEF_base_work_directory+definitions.data_file_folder_list[i]+"\\"+definitions.data_file_list[i], i);
				System.out.println("Indexed file: #"+(i+1)+" - time passed so far: "+(((new Date()).getTime() - start.getTime())/1000.0 + " total seconds"));
			}
		}
		
		private void parse_entire_file(IndexWriter writer, String file_path, int file_number)
		{
			long current_file_pos = 0;
			
			try(BufferedReader br = new BufferedReader(new FileReader(new File(file_path)), 1024*1024*8))//buffer 8MB blocks
			{
			    for(String line; (line = br.readLine()) != null; )
			    {
			    	JsonParser jsp = new JsonParser();
			    	index_comment(writer, extract_json_body(line, jsp), file_number, current_file_pos);
			    	current_file_pos += line.length()+1;
			    }
			}
			catch (Exception e)
			{
				e.printStackTrace();
			}
		}
		
		private String extract_json_body(String json_line, JsonParser jsp)
		{
		    JsonElement jelement = jsp.parse(json_line);
		    JsonObject  jobject = jelement.getAsJsonObject();
		    return jobject.get("body").getAsString();
		}
		
		private void index_comment(IndexWriter writer, String comment, int file_no, long file_pos) throws IOException
		{
			Document doc = new Document();
			
			doc.add(new TextField("location", file_no+"-"+file_pos, Field.Store.YES));//since we don't store the comment (to improve speed), we need to store the location of the comment on the input file, so we can retrieve it later
			doc.add(new TextField("body", comment, Field.Store.NO));
			writer.addDocument(doc);
		}
	}
}
