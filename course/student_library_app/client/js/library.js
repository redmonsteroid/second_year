document.addEventListener("DOMContentLoaded", () => {
    loadBooks();
  });
  
  // Загрузка книг
async function loadBooks() {
  const token = localStorage.getItem("token");
  if (!token) {
    alert("Unauthorized. Please login.");
    window.location.href = "login.html";
    return;
  }

  try {
    const response = await fetch("http://127.0.0.1:8000/books/", {
      headers: { Authorization: `Bearer ${token}` },
    });

    if (!response.ok) {
      throw new Error("Failed to load books");
    }

    const books = await response.json();
    displayBooks(books);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

// Отображение книг
function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  bookList.innerHTML = "";

  books.forEach((book) => {
    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${book.title} <br>
      <strong>Authors:</strong> ${book.authors.length > 0 ? book.authors.map(a => `${a.last_name} ${a.first_name} ${a.middle_name || ''}`).join(", ") : "N/A"} <br>
      <strong>Publisher:</strong> ${book.publisher || "N/A"} <br>
      <strong>Publication City:</strong> ${book.publication_city || "N/A"} <br>
      <strong>Year:</strong> ${book.publication_year || "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count || "N/A"} <br>
      <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
      <strong>Download Link:</strong> ${book.download_link || "N/A"} <br>
      <strong>Teacher:</strong> ${book.owner_username || "N/A"} <br>
    `;
    bookList.appendChild(li);
  });
}