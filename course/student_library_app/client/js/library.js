let booksData = [];                 // Исходный массив всех книг
const uniqueValuesByField = {};     // Словарь множеств уникальных значений по каждому полю

document.addEventListener("DOMContentLoaded", () => {
  loadBooks();

  // Элементы
  const filterTypeSelect = document.getElementById("filter-type");
  const dropdownToggleBtn = document.getElementById("dropdown-toggle-btn");
  const dropdownContent = document.getElementById("dropdown-content");
  const dropdownSearch = document.getElementById("dropdown-search");
  const applyFilterBtn = document.getElementById("apply-filter-btn");
  const sortBtn = document.getElementById("sort-btn");

  // При смене "Filter by" — генерируем чекбоксы
  filterTypeSelect.addEventListener("change", () => {
    // Скрываем/сбрасываем меню при смене типа
    dropdownContent.classList.remove("show");
    onFilterTypeChange();
  });

  // Кнопка раскрытия/скрытия дропдауна
  dropdownToggleBtn.addEventListener("click", () => {
    dropdownContent.classList.toggle("show");
  });

  // Поиск по чекбоксам внутри дропдауна
  dropdownSearch.addEventListener("input", () => {
    filterCheckBoxList(dropdownSearch.value.trim());
  });

  // Применение фильтра
  applyFilterBtn.addEventListener("click", applyFilter);

  // Сортировка
  sortBtn.addEventListener("click", applySort);

  // Если пользователь кликает вне дропдауна, мы закрываем меню
  document.addEventListener("click", (event) => {
    if (!dropdownContent.contains(event.target) && !dropdownToggleBtn.contains(event.target)) {
      dropdownContent.classList.remove("show");
    }
  });
});

/**
 * Загрузка книг с сервера
 */
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

    booksData = await response.json();
    // Формируем уникальные значения
    buildUniqueValues(booksData);

    // Показываем все книги
    displayBooks(booksData);
  } catch (error) {
    console.error(error);
    alert("Error loading books");
  }
}

/**
 * Собираем уникальные значения (authors, teacher, publisher, city, year, title)
 */
function buildUniqueValues(books) {
  uniqueValuesByField.authors = new Set();
  uniqueValuesByField.teacher = new Set();
  uniqueValuesByField.publisher = new Set();
  uniqueValuesByField.publication_city = new Set();
  uniqueValuesByField.publication_year = new Set();
  uniqueValuesByField.title = new Set();

  books.forEach((book) => {
    // authors
    if (book.authors && Array.isArray(book.authors)) {
      book.authors.forEach((a) => {
        const fullName = `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`;
        uniqueValuesByField.authors.add(fullName.trim());
      });
    }
    // teacher
    if (book.owner_username) uniqueValuesByField.teacher.add(book.owner_username);
    // publisher
    if (book.publisher) uniqueValuesByField.publisher.add(book.publisher);
    // city
    if (book.publication_city) uniqueValuesByField.publication_city.add(book.publication_city);
    // year
    if (book.publication_year) uniqueValuesByField.publication_year.add(String(book.publication_year));
    // title
    if (book.title) uniqueValuesByField.title.add(book.title);
  });

  // Превращаем множества в массивы
  for (let field in uniqueValuesByField) {
    uniqueValuesByField[field] = Array.from(uniqueValuesByField[field]).sort();
  }
}

/**
 * Когда выбираем новое поле в "Filter by"
 * -> Перезаполняем чекбоксы в дропдауне
 */
function onFilterTypeChange() {
  const filterType = document.getElementById("filter-type").value;
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");
  const dropdownSearch = document.getElementById("dropdown-search");

  // Очищаем
  dropdownCheckboxList.innerHTML = "";
  dropdownSearch.value = ""; // сбрасываем поисковое поле

  if (!filterType) {
    // Если ничего не выбрано, выходим
    return;
  }

  const uniqueValues = uniqueValuesByField[filterType] || [];

  uniqueValues.forEach((val) => {
    const label = document.createElement("label");
    const cb = document.createElement("input");
    cb.type = "checkbox";
    cb.value = val;
    label.appendChild(cb);
    label.appendChild(document.createTextNode(val));
    dropdownCheckboxList.appendChild(label);
  });

  // По умолчанию скрыто, пользователь сам нажмёт на кнопку, чтобы раскрыть
}

/**
 * Фильтр чекбоксов внутри дропдауна (поиском)
 */
function filterCheckBoxList(searchStr) {
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");
  const labels = dropdownCheckboxList.querySelectorAll("label");

  labels.forEach((lbl) => {
    const text = lbl.textContent.toLowerCase();
    if (text.includes(searchStr.toLowerCase())) {
      lbl.style.display = "";
    } else {
      lbl.style.display = "none";
    }
  });
}

/**
 * Применяем фильтр (по выбранному полю и отмеченным чекбоксам)
 */
function applyFilter() {
  const filterType = document.getElementById("filter-type").value;
  const dropdownCheckboxList = document.getElementById("dropdown-checkbox-list");

  if (!filterType) {
    displayBooks(booksData);
    return;
  }

  // Собираем отмеченные чекбоксы
  const checkedValues = Array.from(
    dropdownCheckboxList.querySelectorAll("input[type='checkbox']:checked")
  ).map(cb => cb.value);

  if (checkedValues.length === 0) {
    // Если ни один чекбокс не выбран, показываем всё
    displayBooks(booksData);
    return;
  }

  let filtered = booksData.filter((book) => {
    switch (filterType) {
      case "authors":
        // Проверяем, есть ли пересечение с авторами
        if (!book.authors) return false;
        const authorsFullNames = book.authors.map((a) => {
          return (a.last_name + " " + a.first_name + (a.middle_name ? " " + a.middle_name : "")).trim();
        });
        return authorsFullNames.some((name) => checkedValues.includes(name));

      case "teacher":
        return checkedValues.includes(book.owner_username);
      case "publisher":
        return checkedValues.includes(book.publisher);
      case "publication_city":
        return checkedValues.includes(book.publication_city);
      case "publication_year":
        return checkedValues.includes(String(book.publication_year));
      case "title":
        return checkedValues.includes(book.title);

      default:
        return true;
    }
  });

  displayBooks(filtered);
}

/**
 * Отображение книг
 */
function displayBooks(books) {
  const bookList = document.getElementById("book-list");
  if (!bookList) return;
  bookList.innerHTML = "";

  books.forEach((book) => {
    const li = document.createElement("li");
    li.innerHTML = `
      <strong>Title:</strong> ${book.title || "N/A"} <br>
      <strong>Authors:</strong> ${
        book.authors && book.authors.length > 0
          ? book.authors
              .map(
                (a) => 
                  `${a.last_name} ${a.first_name}${a.middle_name ? " " + a.middle_name : ""}`
              )
              .join(", ")
          : "N/A"
      } <br>
      <strong>Publisher:</strong> ${book.publisher || "N/A"} <br>
      <strong>Publication City:</strong> ${book.publication_city || "N/A"} <br>
      <strong>Year:</strong> ${book.publication_year || "N/A"} <br>
      <strong>Pages:</strong> ${book.page_count || "N/A"} <br>
      <strong>Additional Info:</strong> ${book.additional_info || "N/A"} <br>
      <strong>Download Link:</strong> ${
        book.download_link 
          ? `<a href="${book.download_link}" target="_blank">${book.download_link}</a>`
          : "N/A"
      } <br>
      <strong>Teacher:</strong> ${book.owner_username || "N/A"} <br>
    `;
    bookList.appendChild(li);
  });
}

/**
 * Сортировка (как раньше)
 */
function applySort() {
  const sortType = document.getElementById("sort-type").value;
  if (!sortType) {
    displayBooks(booksData);
    return;
  }

  let sorted = [...booksData];

  switch (sortType) {
    case "title":
      sorted.sort((a, b) => (a.title || "").localeCompare(b.title || ""));
      break;
    case "authors":
      sorted.sort((a, b) => {
        const aAuthor = (a.authors && a.authors[0]) 
          ? a.authors[0].last_name.toLowerCase() 
          : "";
        const bAuthor = (b.authors && b.authors[0]) 
          ? b.authors[0].last_name.toLowerCase() 
          : "";
        return aAuthor.localeCompare(bAuthor);
      });
      break;
    case "publication_year":
      sorted.sort((a, b) => (b.publication_year || 0) - (a.publication_year || 0));
      break;
    case "teacher":
      sorted.sort((a, b) => {
        const aTeacher = (a.owner_username || "").toLowerCase();
        const bTeacher = (b.owner_username || "").toLowerCase();
        return aTeacher.localeCompare(bTeacher);
      });
      break;
    default:
      break;
  }

  displayBooks(sorted);
}

/**
 * Выход (logout)
 */
function logout() {
  localStorage.removeItem("token");
  localStorage.removeItem("role");
  window.location.href = "login.html";
}
